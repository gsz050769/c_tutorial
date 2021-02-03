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
#include "libjson_array.h"

ljs * ljs_array_get_index(ljs *array, int idx)
{
	

	int count=0;	
	int number=0;

	
	//printf("[LJS_ARRAY] %s array=%p idx=%d\n",__FUNCTION__,array,idx);
	number=ljs_read_anz_level_elements(array->child);
	idx=abs(idx);
	if(idx>number)
	{
		return NULL;;
	}
	
	if(array && array->type==ljsType_array && array->child)
	{
		array=array->child;
		if(array)
		{
			// skip root element
			array=array->next;
		}
		while(array)
		{
			if(count==idx)
			{
				return array;
			}
			count++;
			array=array->next;
		}
	}
	return NULL;
}


ljs * ljs_array_create_index_of_null(ljs *array, int idx)
{
	int count=0;
	
	idx=abs(idx);

	//printf("[LJS_ARRAY] %s array=%p idx=%d\n",__FUNCTION__,array,idx);

	if(array && array->type==ljsType_array && array->child)
	{
		array=array->child;
		while(array)
		{
			if(count==idx+1)
			{
				return array;
			}
			if(array->next==NULL)
			{
				array->next=ljs_init();
				array->next->type=ljsType_null;
				array->next->prev=array;
				char *buf=malloc(33);
				sprintf(buf,"%d",count);
				array->next->key=buf;
			}
			count++;
			array=array->next;
		}
	}
	return NULL;
}

ljs * ljs_array_create_next_index_of_null(ljs *array_entry)
{
	//printf("[LJS_ARRAY] %s 1 %p\n",__FUNCTION__,array_entry); 
	ljs* array=ljs_read_get_parent(array_entry);
	int entries=0;
	if(array_entry && array)
	{
		entries = ljs_read_anz_level_elements(array_entry);
		return (ljs_array_create_index_of_null(array,entries));
	}
	return NULL;
}