/**
 *    Json libarary (jlib) used to create and read JSON messages in an eay way.
 *    Copyright (C) 2021  Georg Schmitz
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h> 
#include <stdlib.h> 
#include <string.h>

#include "libjson.h"
#include "libjson_print.h"
#include "libjson_array.h"

typedef enum ljs_parse_state
{
    s_idle,             // 0
    s_object_key,       // 1
    s_object_colon,     // 2
    s_object_value,     // 3
    s_object_comma,     // 4
    s_array_value,      // 5
    s_array_comma,      // 6
    s_error,            // 7
} ljs_parse_state;

static ljs_parse_state state_stack[100];
static int state_idx=0;
static void ljs_parse_init_state(void)
{
    state_idx=0;
    state_stack[state_idx]=s_idle;
}
static void ljs_parse_set_state(ljs_parse_state new_state)
{
  state_stack[state_idx]=new_state;
  //printf("[LJS_PARSE] %s new state = %d\n",__FUNCTION__,new_state);
}
static void ljs_parse_push_state(ljs_parse_state new_state)
{
    state_idx++;
    if(state_idx<100)
    {
        state_stack[state_idx]=new_state;
    }
    else
    {
        state_idx--;
        state_stack[state_idx]=s_error;
    }
    //printf("[LJS_PARSE] %s new state = %d\n",__FUNCTION__,state_stack[state_idx]);
}
static void ljs_parse_pop_state(void)
{
    state_idx--;
    if(state_idx<0)
    {
        state_idx++;
        state_stack[state_idx]=s_error;
    }
    //printf("[LJS_PARSE] %s new state = %d\n",__FUNCTION__,state_stack[state_idx]);
}
static ljs_parse_state  ljs_parse_get_state(void)
{
    return (state_stack[state_idx]);
}

typedef enum ljs_parse_type
{
    ljs_pt_obj_start,
    ljs_pt_obj_end,
    ljs_pt_array_start,
    ljs_pt_array_end,
    ljs_pt_string,
    ljs_pt_null,
    ljs_pt_true,
    ljs_pt_false,
    ljs_pt_number,
    ljs_pt_comma,
    ljs_pt_colon,
    ljs_pt_other     // error
} ljs_parse_type;

typedef struct ljs_parse_array_type
{
  ljs_parse_type type;
  bool vlaid;
} ljs_parse_array_type;
struct ljs_parse_array_type array_valid_type_tab[] =
{

    {ljs_pt_obj_start,   1},
    {ljs_pt_obj_end,     0},
    {ljs_pt_array_start, 1},
    {ljs_pt_array_end,   0},
    {ljs_pt_string,      1},
    {ljs_pt_null,        1},
    {ljs_pt_true,        1},
    {ljs_pt_false,       1},
    {ljs_pt_number,      1},
    {ljs_pt_comma,       0},
    {ljs_pt_colon,       0},
    {ljs_pt_other,       0 }
};


static double number_value=0;
static ljs_parse_type ljs_parse_get_next_element(char * msg, int * start, int* len)
{
    int idx=0;
    char * end_of_number=NULL;

    //printf("[LJS_PARSE] %s input = %s\n",__FUNCTION__, msg);
    while (msg[idx]!=0)
    {
        *start=idx; // default return value
        *len=1;     // default return value
        switch(msg[idx])
        {
            case ' ':
                break;
            case '{':
                return ljs_pt_obj_start;
            case '}':
                return ljs_pt_obj_end;
            case '[':
                return ljs_pt_array_start;
            case ']':
                return ljs_pt_array_end;
            case ',':
                return ljs_pt_comma;
            case ':':
                return ljs_pt_colon;
            case '"':
                *start=idx+1;
                while(msg[++idx])
                {
                    if(msg[idx]=='"')
                    {
                        *len=idx-*start+1;
                        return ljs_pt_string;
                    }
                }
                break;
            case 'n':
                if(strncmp(&msg[idx],"null",strlen("null"))==0)
                {
                    *start=idx;
                    *len=strlen("null");
                    return ljs_pt_null;
                }
                break;
            case 't':
                if(strncmp(&msg[idx],"true",strlen("true"))==0)
                {
                    *start=idx;
                    *len=strlen("true");
                    return ljs_pt_true;
                }
                break;
            case 'f':
                if(strncmp(&msg[idx],"false",strlen("false"))==0)
                {
                    *start=idx;
                    *len=strlen("false");
                    return ljs_pt_false;
                }
                break;
            default:
                number_value=strtod(&msg[idx], &end_of_number);
                if((end_of_number) &&(end_of_number > &msg[idx]))
                {
                    *start=idx;
                    *len=end_of_number-&msg[idx];
                    return ljs_pt_number;
                }
                break;

        }
        idx++;
    }
    return ljs_pt_other; // error
}

ljs * ljs_parse_from_string(char *in)
{
    
    int idx=0;
    int start=0;
    int len=0;
    ljs * js=NULL;
    ljs * js_start=NULL;

    //printf("[LJS_PARSE] %s input = %s len =%d\n",__FUNCTION__,in, (int)strlen(in));
    ljs_parse_init_state();

    if(!in)
    {
        return NULL;
    }

    int oldidx=-1;
    while (in[idx]&&(ljs_parse_get_state()!=s_error))
    {
        if (oldidx==idx)
        {
            ljs_parse_set_state(s_error);  // for security reasons, preventing endless loop
            break;
        }
        oldidx=idx;
        switch(ljs_parse_get_state())
        {
            case s_idle: // start
                switch (ljs_parse_get_next_element(&in[idx],&start,&len))
                {
                    case ljs_pt_obj_start:
                        idx=idx+len;
                        js=ljs_init();
                        js_start=js;
                        ljs_parse_push_state(s_object_key);
                        break;
                    default:
                        ljs_parse_set_state(s_error);
                        break;
                }
                break;
            case s_object_key:  // { ->
                switch(ljs_parse_get_next_element(&in[idx],&start,&len))
                {
                    case ljs_pt_obj_end:
                        idx+=start+len;
                        ljs_parse_pop_state();
                        break;
                    case ljs_pt_string:
                        js->next=ljs_init();
                        js->next->prev=js;
                        js=js->next;
                        js->key=malloc(len);
                        strncpy(js->key,&in[idx]+start,len-1);
                        idx+=start+len;
                        ljs_parse_set_state(s_object_colon);
                        break;
                    default:
                        ljs_parse_set_state(s_error);
                        break;
                }
                break;
            case s_object_colon:  // { "hkjhkj" ->
                switch(ljs_parse_get_next_element(&in[idx],&start,&len))
                {
                    case ljs_pt_colon:
                        idx+=start+len;
                        ljs_parse_set_state(s_object_value);
                        break;
                    default:
                        ljs_parse_set_state(s_error);
                        break;
                }
                break;
            case s_object_value: // { "hhhh" : ->
                switch(ljs_parse_get_next_element(&in[idx],&start,&len))
                {
                    case ljs_pt_string:
                        js->strVal=malloc(len);
                        strncpy(js->strVal,&in[idx]+start,len-1);
                        js->type=ljsType_string;
                        idx+=start+len;
                        ljs_parse_set_state(s_object_comma);
                        break;
                    // To Do other types
                    case ljs_pt_null:
                        idx+=start+len;
                        js->type=ljsType_null;
                        ljs_parse_set_state(s_object_comma);
                        break;

                    case ljs_pt_true:
                        idx+=start+len;
                        js->type=ljsType_bool;
                        js->boolean=1;
                        ljs_parse_set_state(s_object_comma);
                        break;

                    case ljs_pt_false:
                        idx+=start+len;
                        js->type=ljsType_bool;
                        js->boolean=0;
                        ljs_parse_set_state(s_object_comma);
                        break;
                    case ljs_pt_number:
                        idx+=start+len;
                        js->type=ljsType_number;
                        js->number=number_value;
                        ljs_parse_set_state(s_object_comma);
                        break;
                    case ljs_pt_obj_start:
                        idx+=start+len;
                        js->type=ljsType_object;
                        js->child=ljs_init();
                        js->child->prev=js;
                        js=js->child;
                        ljs_parse_set_state(s_object_comma);
                        ljs_parse_push_state(s_object_key);
                        break;
                    case ljs_pt_array_start:
                        idx+=start+len;
                        js->type=ljsType_array;
                        js->child=ljs_init();
                        js->child->prev=js;
                        js=js->child;
                        ljs_parse_set_state(s_object_comma);
                        ljs_parse_push_state(s_array_value);
                        break;
                    default:
                        ljs_parse_set_state(s_error);
                        break;
                }
                break;

            case s_object_comma: // { "hhhh" : "jljljk" ->
                switch(ljs_parse_get_next_element(&in[idx],&start,&len))
                {
                    case ljs_pt_comma: 
                        idx+=start+len;
                        if (ljs_parse_get_next_element(&in[idx],&start,&len)==ljs_pt_string)
                        {
                            ljs_parse_set_state(s_object_key);
                        }
                        else
                        {
                            ljs_parse_set_state(s_error);
                        }
                        break;
                    case ljs_pt_obj_end:
                        idx+=start+len;
                        ljs_parse_pop_state();
                        break;

                    default:
                        ljs_parse_set_state(s_error);
                        break;
                }
                break;

            case s_array_value:
                switch(ljs_parse_get_next_element(&in[idx],&start,&len))
                {
                    case ljs_pt_array_end:
                        idx+=start+len;
                        ljs_parse_pop_state();
                        break;
                    case ljs_pt_string:
                        js->next=ljs_array_create_next_index_of_null(js);
                        js=js->next;
                        js->strVal=malloc(len);
                        strncpy(js->strVal,&in[idx]+start,len-1);
                        js->type=ljsType_string;
                        idx+=start+len;
                        ljs_parse_set_state(s_array_comma);
                        break;
                    // To Do other types
                    case ljs_pt_null:
                        js->next=ljs_array_create_next_index_of_null(js);
                        js=js->next;
                        idx+=start+len;
                        js->type=ljsType_null;
                        ljs_parse_set_state(s_array_comma);
                        break;

                    case ljs_pt_true:
                        js->next=ljs_array_create_next_index_of_null(js);
                        js=js->next;
                        idx+=start+len;
                        js->type=ljsType_bool;
                        js->boolean=1;
                        ljs_parse_set_state(s_array_comma);
                        break;

                    case ljs_pt_false:
                        js->next=ljs_array_create_next_index_of_null(js);
                        js=js->next;
                        idx+=start+len;
                        js->type=ljsType_bool;
                        js->boolean=0;
                        ljs_parse_set_state(s_array_comma);
                        break;
                    case ljs_pt_number:
                        js->next=ljs_array_create_next_index_of_null(js);
                        js=js->next;
                        idx+=start+len;
                        js->type=ljsType_number;
                        js->number=number_value;
                        ljs_parse_set_state(s_array_comma);
                        break;
                    case ljs_pt_obj_start:
                        js->next=ljs_array_create_next_index_of_null(js);
                        js=js->next;
                        idx+=start+len;
                        js->type=ljsType_object;
                        js->child=ljs_init();
                        js->child->prev=js;
                        js=js->child;
                        ljs_parse_set_state(s_array_comma);
                        ljs_parse_push_state(s_object_key);
                        break;
                    case ljs_pt_array_start:
                        js->next=ljs_array_create_next_index_of_null(js);
                        js=js->next;
                        idx+=start+len;
                        js->type=ljsType_array;
                        js->child=ljs_init();
                        js->child->prev=js;
                        js=js->next;
                        ljs_parse_set_state(s_array_comma);
                        ljs_parse_push_state(s_array_value);
                        break;
                    default:
                        ljs_parse_set_state(s_error);
                        break;
                }
                break;
            case s_array_comma:
                switch(ljs_parse_get_next_element(&in[idx],&start,&len))
                {
                    case ljs_pt_comma: 
                        idx+=start+len;
                        if (array_valid_type_tab[ljs_parse_get_next_element(&in[idx],&start,&len)].vlaid)
                        {
                            ljs_parse_set_state(s_array_value);
                        }
                        else
                        {
                            ljs_parse_set_state(s_error);
                        }
                        break;
                    case ljs_pt_array_end:
                        idx+=start+len;
                        ljs_parse_pop_state();
                        break;

                    default:
                        ljs_parse_set_state(s_error);
                        break;
                }
                break;
            default:
                ljs_parse_set_state(s_error);
        }
    }
    if((ljs_parse_get_state()==s_idle) &&(state_idx==0))
    {
        //printf("[LJS_PARSE] %s return = %p \n",__FUNCTION__,js_start);
        return js_start;
    }
    // todo free everything
    ljs_free(js_start);
    //printf("[LJS_PARSE] %s return = NULL \n",__FUNCTION__);
    return NULL;
}