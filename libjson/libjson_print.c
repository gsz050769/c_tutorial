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
