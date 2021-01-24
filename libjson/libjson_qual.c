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

static char * start=NULL;
static char *tuple=NULL;



static char * pointerStart=NULL;
static char * pointerRead=NULL;
static int    length=0;

static char qualString[200]; // max size of qualifier string
int ljs_qual_get_next(char* qual, ljsQualTuple * retTuple)
{
	// example "dm:ljsType_object/mac:ljsType_string"
	char *help;
	if (qual)
	{
		length=strlen(qual);
		pointerStart=qualString;
		pointerRead=qualString+sizeof(qualString);
		if(length<sizeof(qualString))
		{
			memset(qualString,0,sizeof(qualString));
			strncpy(qualString,qual,length);
			pointerStart = qualString;
			pointerRead  = qualString;
		}
	}
	retTuple->key=pointerRead;
	while(pointerRead<=(pointerStart+length))
	{
		if ((*pointerRead)==58)
		{
			*pointerRead=0;
			pointerRead++;
			help=pointerRead;
			while(pointerRead<=(pointerStart+length))
			{
				if ((*pointerRead=='/')||(*pointerRead==0))
				{
					*pointerRead=0;
					pointerRead++;
					if (strcmp(help,"ljsType_root")==0)
					{
						retTuple->jstype=ljsType_root;
						return 0;
					}
					if (strcmp(help,"ljsType_null")==0)
					{
						retTuple->jstype=ljsType_null;
						return 0;
					}
					if (strcmp(help,"ljsType_bool")==0)
					{
						retTuple->jstype=ljsType_bool;
						return 0;
					}
					if (strcmp(help,"ljsType_object")==0)
					{
						retTuple->jstype=ljsType_object;
						return 0;
					}
					if (strcmp(help,"ljsType_array")==0)
					{
						retTuple->jstype=ljsType_array;
						return 0;
					}
					if (strcmp(help,"ljsType_number")==0)
					{
						retTuple->jstype=ljsType_number;
						return 0;
					}
					if (strcmp(help,"ljsType_string")==0)
					{
						retTuple->jstype=ljsType_string;
						return 0;
					}
					if (retTuple->jstype!=ljsType_root)
					{
						return 0;
					}	
				}
			pointerRead++;
			}

		}
		pointerRead++;
	}
	return -1;
}
