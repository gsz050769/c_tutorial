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

int ljs_read_anz_level_elements(ljs *js)
{
	int count=0;
	//printf("[LJS_READ] %s start %p \n",__FUNCTION__,js);
	while(js && (js->type!=ljsType_root))
	{
		js=js->prev;
	}
	js=js->next;
	while(js)
	{
		count++;
		js=js->next;
	}
	return count;
}

ljs * ljs_read_last_level_element(ljs *js)
{
	ljs *last=NULL;
	while(js)
	{
		last=js;
		js=js->next;
	}
	return last;
}

ljs * ljs_read_element(ljs *js, ljsQualTuple tup)
{

	//printf("[LJS_READ] %s ljs=%p ljsQualTuple.key=%s\n",__FUNCTION__,js,tup.key);
	while(js)
	{
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
	return NULL;
}

int ljs_read(ljs * js, char * qual, void ** result)
{
	ljs * ljs_my=js;
	ljsQualTuple jstuple;
	bool objOrArray=0;
	ljs * last=NULL;
	
	//printf("[LJS_READ] %s start js=%p qual=%s\n",__FUNCTION__,js,qual);

	if(!ljs_my)
	{
		*result=NULL;
		return -1;
	}

    if (ends_with(qual,"ljsType_object")||ends_with(qual,"ljsType_array"))
	{
		objOrArray=1;
	}

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
						*((bool**)result)=&ljs_my->boolean;
						return 0;

					case ljsType_null:
						*((void**)result)=NULL;
						return 0;

					case ljsType_number:
						*((double**)result)=&ljs_my->number;
						return 0;

					case ljsType_string:
						*((char**)result)=ljs_my->strVal;
						return 0;

					case ljsType_object:
					case ljsType_array:
						last=ljs_my;
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
	if(last)
	{
		if(objOrArray)
		{
			*((ljs**)result)=ljs_my;
			return 0;
		}
	}
	return -1;
}

ljs* ljs_read_ref(ljs * js, char * qual)
{
	
	ljs * ljs_my=js;
	ljsQualTuple jstuple;
	bool objOrArray=0;
	ljs * last=NULL;
	
	//printf("[LJS_READ] %s start js=%p qual=%s\n",__FUNCTION__,js,qual);

	if(!ljs_my)
	{
		return NULL;
	}

    if (ends_with(qual,"ljsType_object")||ends_with(qual,"ljsType_array"))
	{
		objOrArray=1;
	}

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
					case ljsType_null:
					case ljsType_number:
					case ljsType_string:
						return ljs_my;
					case ljsType_array:
					case ljsType_object:
						ljs_my=ljs_my->child;
						last=ljs_my;
						break;
					case ljsType_root:
						break;
					default:
						return NULL;
				}
			}
		} while ((ljs_my)&&(0==ljs_qual_get_next(NULL,&jstuple)));
	}
	if(last)
	{
		if(objOrArray)
		{
			return last;
		}
	}
	
	return NULL;
}
