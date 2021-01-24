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

ljsFormat _format=0;
int tab=0;

static void ljs_print_tab(int tab)
{
	int idx=0;

	printf("[LJS] ");
	for(idx=0;idx<tab;idx++)
	{
		printf("\t");
	}
}

void ljs_print_start(void)
{
	tab=1;
	printf("[LJS]{\n");
}

void ljs_print_end(void)
{
	printf("[LJS]}\n");
}

void ljs_print_element(ljs * js,ljsFormat format)
{
	_format=format;
	while(js)
	{
		switch(js->type)
		{
			case ljsType_bool:
				ljs_print_tab(tab);
				printf("\"%s\" : %s",js->key,js->boolean?"true":"false");
				break;
			case ljsType_null:
				ljs_print_tab(tab);
				printf("\"%s\" : null",js->key);
				break;
			case ljsType_number:
				ljs_print_tab(tab);
				printf("\"%s\" : %g",js->key,js->number);
				break;
			case ljsType_string:
				ljs_print_tab(tab);
				printf("\"%s\" : \"%s\"",js->key,js->strVal);
				break;
			case ljsType_object:
			case ljsType_array:
				ljs_print_tab(tab);
				printf("\"%s\" : %s\n",js->key,js->type==ljsType_object?"{":"[");
				tab++;
				ljs_print_element(js->child,format);
				tab--;
				ljs_print_tab(tab);
				printf("%s",js->type==ljsType_object?"}":"]");
				break;
			case ljsType_root:
				break;
		}
		if(js->type!=ljsType_root)
		{
			if(js->next)
			{
				printf(",");
			}
			printf("\n");
		}
		js=js->next;
	}
}

void ljs_print_pointers(ljs * js)
{
	while(js)
	{
		printf("%p\n",js);
		if(js->child)
		{
			printf("->\n");
			ljs_print_pointers(js->child);
			printf("<-\n");
		}
		js=js->next;
	}
}


unsigned long size;
unsigned long js_print_sizeof(ljs* js,bool start)
{
	if(start)
	{
		size=0; //'{}'
	}
	while(js)
	{
		switch(js->type)
		{
			case ljsType_bool:
				size+=strlen("false"); // too much in case it is "true" .. so what :-)
				break;
			case ljsType_null:
				size+=strlen("null");
				break;
			case ljsType_number:
				size+=snprintf(NULL,0,"%g",js->number); // provides size when no buffer anlength is given
				break;
			case ljsType_string:
				size+=((js->strVal!=NULL)?strlen(js->strVal):0)+2;
				printf("[LJS]js_print_sizeof size=%lu\n",size);
				break;
			case ljsType_object:
			case ljsType_array:
				size+=+1; //  '='
				js_print_sizeof(js->child,0); // recursive with start=0
				break;
			case ljsType_root:
				size+=2;// '{}' or '[]'
				break;
		}
		if(js->type!=ljsType_root)
		{
			size+=((js->key!=NULL)?strlen(js->key):0)+3; //2 * '"' + ':' 
			if(js->next)
			{
				size++; // ",");
			}
		}
		js=js->next;
	}
	return size;
}


char *ljs_print_write=NULL;
unsigned long size_of_js=0;
char *ljs_out_buf=NULL;

static char  * _ljs_print_malloc_element(ljs *js,bool start)
{

	if(start)
	{
		size_of_js=js_print_sizeof(js,1)+100;  // +1 for string termination + 99 secrity space
		ljs_out_buf=malloc(size_of_js);
		ljs_print_write=ljs_out_buf;
		sprintf(ljs_print_write,"{");
	}
	if(!ljs_out_buf)
	{
		return (NULL);
	}
	ljs_print_write=ljs_out_buf+strlen(ljs_out_buf);
	while(js&&(ljs_print_write<(ljs_out_buf+size_of_js)))
	{
		ljs_print_write=ljs_out_buf+strlen(ljs_out_buf);
		switch(js->type)
		{
			case ljsType_bool:
				sprintf(ljs_print_write,"\"%s\":%s",js->key,js->boolean?"true":"false");
				break;
			case ljsType_null:
				sprintf(ljs_print_write,"\"%s\":null",js->key);
				break;
			case ljsType_number:
				sprintf(ljs_print_write,"\"%s\":%g",js->key,js->number);
				break;
			case ljsType_string:
				sprintf(ljs_print_write,"\"%s\":\"%s\"",js->key,js->strVal);
				break;
			case ljsType_object:
			case ljsType_array:
				sprintf(ljs_print_write,js->type==ljsType_object?"\"%s\":{":"\"%s\":[",js->key);
				_ljs_print_malloc_element(js->child,0);
				strcat(ljs_out_buf,js->type==ljsType_object?"}":"]");
			case ljsType_root:
				break;
		}
		if(js->type!=ljsType_root)
		{
			if(js->next)
			{
				ljs_print_write=ljs_out_buf+strlen(ljs_out_buf);
				sprintf(ljs_print_write,",");
			}
		}
		js=js->next;
	}
	ljs_print_write=ljs_out_buf+strlen(ljs_out_buf);
	return ljs_out_buf;
}

char  * ljs_print_malloc_element(ljs *js)
{
	char * buf=_ljs_print_malloc_element(js,1);;
	if(buf)
	{
		strcat(buf,"}");
	}
	return buf;
}
