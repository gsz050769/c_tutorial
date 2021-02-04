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
#include "libjson_qual.h"
#include "libjson_write.h"
#include "libjson_read.h"
#include "libjson_array.h"
#include "libjson_memory.h"

char int_buf[20];

static ljs * ljs_write_get_element(ljs *js, ljsQualTuple tup)
{
	ljs * lastljs=ljs_read_last_level_element(js);
	bool parent_is_array=0;

	//printf("[LJS_WRITE] %s start %p tup.key=%s\n",__FUNCTION__,js,tup.key);
	if (ljs_read_get_parent_type(js)==ljsType_array)
	{
		js=ljs_read_get_parent(js);
		if(js)
		{
			if (NULL!=ljs_array_get_index(js, atoi(tup.key)))
			{
				return ljs_array_get_index(js, atoi(tup.key));
			}
			else
			{
				if (NULL!=(js=ljs_array_create_index_of_null(js, atoi(tup.key))))
				{
					js->type=tup.jstype;
					return js;
				}
				else
				{
					return NULL;
				}
			}
			
		}
	}
	while(js)
	{
		lastljs=js;
		if (js->key && (0==strcmp(js->key,tup.key)))
		{
			if(js->type!=tup.jstype)
			{
				return NULL;
			}
			return js;
		}
		js=js->next;
	}
	if(lastljs)
	{
		js=ljs_init();
		lastljs->next=js;
		js->prev=lastljs;
		js->type=tup.jstype;
        js->key=libjson_malloc(strlen(tup.key)+1);
		strcpy(js->key,tup.key);
		return js;
	}
	return NULL;
}


int ljs_write(ljs *js, char* qual, void* val)
{
	ljs * ljs_my=js;
	ljsQualTuple jstuple;
	
	if(!ljs_my)
		return -1;

	if (0==ljs_qual_get_next(qual,&jstuple))
	{
		do
		{
			if(NULL!=(ljs_my=ljs_write_get_element(ljs_my,jstuple))) // get element (created if not available)
			{
				// set value depeding on jason object type
				switch(jstuple.jstype)
				{
					case ljsType_bool:
						ljs_my->boolean= *((bool*) val) !=0 ? true:false;
						return 0;

					case ljsType_null:
						return 0;

					case ljsType_number:
						ljs_my->number= *((double*) val);
						return 0;

					case ljsType_string:
						ljs_my->strVal= libjson_malloc(strlen((char*) val)+1);
						strcpy(ljs_my->strVal,(char*) val);
						return 0;

					case ljsType_object:
					case ljsType_array:
						if(ljs_my->child==NULL)
						{
							ljs_my->child=ljs_init();
							ljs_my->child->prev=ljs_my;
						}
						ljs_my=ljs_my->child;
						break;
					case ljsType_root:
						break;
					default:
						return -1;
				}
			}
		} while ((ljs_my)&&(0==ljs_qual_get_next(NULL,&jstuple)));
	}
	return -1;
}

