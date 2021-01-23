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
#include "libjson_free.h"

int ljs_free_object(ljs *js, bool start)
{
	ljs * js_next=NULL;
	printf("[LJS] free start %p \n",js);
	while (js!=NULL)
	{
		printf("[LJS_FREE] ljs while loop %p \n",js);
		if (js->child!=NULL)
		{
			printf("[LJS_FREE] ljs free recursive %p js->key\n",js->child);
			ljs_free_object(js->child,0);
		}
		
		printf("[LJS_FREE] ljs free instance %p \n",js);
		if(js->key!=NULL)
		{
			printf("[LJS_FREE] ljs free key %s p=%p\n",js->key,js->key);
			free(js->key);
		}
		if(js->strVal!=NULL)
		{
			printf("[LJS_FREE] ljs free strval %s p=%p\n",js->strVal,js->strVal);
			free(js->strVal);
		}
		js_next=js->next;
		if (js->next)
		{
			js->next->prev=js->prev;
		}
		if(js->type==ljsType_root)
		{
			start=0;
		}
		printf("[LJS_FREE] js %p next %p start %d\n",js,js->next,start);

		printf("[LJS_FREE] ljs free object %p \n",js);
		free(js);

		js=NULL;
		if (!start)
		{
			js=js_next;
			start=0;
		}
	}
	return 0;
}
