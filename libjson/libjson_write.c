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
#include "libjson_write.h"
#include "libjson_qual.h"

static ljs * ljs_get_element(ljs *js, ljsQualTuple tup)
{
	ljs * lastljs=NULL;

	printf("[LJS_WRITE] %s ljs=%p ljsQualTuple.key=%s\n",__FUNCTION__,js,tup.key);
	while(js)
	{
		lastljs=js;
		if (js->key && (0==strcmp(js->key,tup.key)))
		{
			if(js->type!=tup.jstype)
			{
				printf("[LJS_WRITE] %s return 0\n",__FUNCTION__);
				return NULL;
			}
			printf("[LJS_WRITE] %s return js=%p\n",__FUNCTION__,js);
			return js;
		}
		js=js->next;
		printf("[LJS_WRITE] %s js++=%p\n",__FUNCTION__,js);
	}
	if(lastljs)
	{
		js=ljs_init();
		lastljs->next=js;
		js->prev=lastljs;
		js->type=tup.jstype;
                js->key=malloc(strlen(tup.key)+1);
		strcpy(js->key,tup.key);
		printf("[LJS_WRITE] %s new js object =%p type=%d, key=%s prev=%p\n",__FUNCTION__,js,js->type,js->key,lastljs);
		return js;
	}
	return NULL;
}


int ljs_write(ljs *js, char* qual, void* val)
{
	ljs * ljs_my=js;
	ljsQualTuple jstuple;
	
	printf("[LJS_WRITE] %s start js=%p qual=%s\n",__FUNCTION__,js,qual);

	if(!ljs_my)
		return -1;

	if (0==ljs_qual_get_next(qual,&jstuple))
	{
		do
		{
			if(NULL!=(ljs_my=ljs_get_element(ljs_my,jstuple))) // get element (created if not available)
			{
				// set value depeding on jason object type
				switch(jstuple.jstype)
				{
					case ljsType_bool:
						printf("[LJS_WRITE] %s set bool value in %p\n",__FUNCTION__,ljs_my);
						ljs_my->boolean= *((bool*) val) !=0 ? true:false;
						printf("[LJS_WRITE] %s  set bool value in %p to %d\n",__FUNCTION__,ljs_my,ljs_my->boolean);
						return 0;

					case ljsType_null:
						printf("[LJS_WRITE] %s data type null, no value in %p\n",__FUNCTION__,ljs_my);
						return 0;

					case ljsType_number:
						ljs_my->number= *((double*) val);
						printf("[LJS_WRITE] %s data type number, in %p to %f\n",__FUNCTION__,ljs_my,ljs_my->number);
						return 0;

					case ljsType_string:
						printf("[LJS_WRITE] %s set string value in %p string=%s\n",__FUNCTION__,ljs_my,(char*) val);
						ljs_my->strVal= malloc(strlen((char*) val)+1);
						strcpy(ljs_my->strVal,(char*) val);
						printf("[LJS_WRITE] %s set string value in %p to %s\n",__FUNCTION__,ljs_my,ljs_my->strVal);
						return 0;

					case ljsType_object:
					case ljsType_array:
						printf("[LJS_WRITE] %s handle object in %p\n",__FUNCTION__,ljs_my);
						if(ljs_my->child==NULL)
						{
							ljs_my->child=ljs_init();
							ljs_my->child->prev=ljs_my;
						}
						printf("[LJS_WRITE] %s enter object %p parent %p\n",__FUNCTION__,ljs_my->child,ljs_my);
						ljs_my=ljs_my->child;
						break;
					case ljsType_root:
						printf("[LJS_WRITE] %s handle data type root %p\n",__FUNCTION__,ljs_my);
						break;
					default:
						return -1;
				}
			}
			printf("[LJS_WRITE] %s ljs_my %p  %d \n",__FUNCTION__,ljs_my,ljs_my?1:0);
		} while ((ljs_my)&&(0==ljs_qual_get_next(NULL,&jstuple)));
	}
	return -1;
}

