
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
#include "libjson_read.h"

static int ends_with(const char *in, const char *suffix)
{
    if (!in || !suffix)
        return 0;
    int lenin = strlen(in);
    int lensuffix = strlen(suffix);
    if (lensuffix >  lenin)
        return 0;
    return (strncmp(in + lenin - lensuffix, suffix, lensuffix)==0);
}

static ljs * ljs_read_element(ljs *js, ljsQualTuple tup)
{
	ljs * lastljs=NULL;

	printf("[LJS_READ] %s ljs=%p ljsQualTuple.key=%s\n",__FUNCTION__,js,tup.key);
	while(js)
	{
		lastljs=js;
		if (js->key && (0==strcmp(js->key,tup.key)))
		{
			if(js->type!=tup.jstype)
			{
				printf("[LJS_READ] %s return 0\n",__FUNCTION__);
				return NULL;
			}
			printf("[LJS_READ] %s return js=%p\n",__FUNCTION__,js);
			return js;
		}
		js=js->next;
		printf("[LJS_READ] %s js++=%p\n",__FUNCTION__,js);
	}
	return NULL;
}

int ljs_read(ljs * js, char * qual, void ** result)
{
	ljs * ljs_my=js;
	ljsQualTuple jstuple;
	bool objOrArray=0;
	ljs * last=NULL;
	
	printf("[LJS_READ] %s start js=%p qual=%s\n",__FUNCTION__,js,qual);

	if(!ljs_my)
	{
		*result=NULL;
		return -1;
	}

        if (ends_with(qual,"ljsType_object")||ends_with(qual,"ljsType_array"))
	{
		objOrArray=1;
	}
	printf("[LJS_READ] %s objOrArray=%d\n",__FUNCTION__,objOrArray);

	if (0==ljs_qual_get_next(qual,&jstuple))
	{
		do
		{
			if(NULL!=(ljs_my=ljs_read_element(ljs_my,jstuple))) // get element (created if not available)
			{
				// set value depeding on jason object type
				last=NULL;
				switch(jstuple.jstype)
				{
					case ljsType_bool:
						printf("[LJS_READ] %s get bool value in %p val=%d\n",__FUNCTION__,ljs_my,ljs_my->boolean);
						*((bool**)result)=&ljs_my->boolean;
						return 0;

					case ljsType_null:
						printf("[LJS_READ] %s get bool value in %p val=null\n",__FUNCTION__,ljs_my);
						*((void**)result)=NULL;
						return 0;

					case ljsType_number:
						printf("[LJS_READ] %s get number value in %p val=%f\n",__FUNCTION__,ljs_my,ljs_my->number);
						*((double**)result)=&ljs_my->number;
						return 0;

					case ljsType_string:
						printf("[LJS_READ] %s get string value in %p val=%s\n",__FUNCTION__,ljs_my,ljs_my->strVal);
						*((char**)result)=ljs_my->strVal;
						return 0;

					case ljsType_object:
					case ljsType_array:
						last=ljs_my;
						printf("[LJS_READ] %s enter object %p parent %p\n",__FUNCTION__,ljs_my->child,ljs_my);
						ljs_my=ljs_my->child;
						break;
					case ljsType_root:
						printf("[LJS_READ] %s handle data type root %p\n",__FUNCTION__,ljs_my);
						break;
					default:
						return -1;
				}
			}
			printf("[LJS_READ] %s ljs_my %p  %d \n",__FUNCTION__,ljs_my,ljs_my?1:0);
		} while ((ljs_my)&&(0==ljs_qual_get_next(NULL,&jstuple)));
	}
	if(last)
	{
		if(objOrArray)
		{
			printf("[LJS_READ] %s get obj/array value val=%p\n",__FUNCTION__,last);
			*((ljs**)result)=ljs_my;
			return 0;
		}
	}
	return -1;
}

ljs* ljs_read_ref(ljs * js, char * qualifier)
{
	return NULL;
}
