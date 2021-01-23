/**
 *	Json libarary (jlib) used to create and read JSON messages in an eay way.
 *	Copyright (C) 2021  Georg Schmitz
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *
 *	@file libjson.c
 *	@author Georg Schmitz
 *	@version 0.0 21/01/21
 *
 *	jlib is an easy to use json librayr. Json objects can easily reffered to by a
 *	qualifier string of format : 
 *  
 *	"<level1_key>:<ljsType_xxx>/<level1_key>:<ljsType_xxx>/..."
 *
 *	example given:  
 * 
 *		ljs_add_string(my_json,"person:ljsType_object/Adresse:ljsType_object/Strasse:ljsType_string","Bond Street");
 *  
 *		creates this JSON content
 * 
 *		{
 *			"person" : {
 *				"Adresse" : {
 *					"Strasse" : "Bond Street"
 *				}
 *			}
 *  
 *		}
 *
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h> 
#include <stdlib.h> 

#include "libjson.h"
#include "libjson_qual.h"
#include "libjson_write.h"
#include "libjson_print.h"
#include "libjson_free.h"
#include "libjson_read.h"


/**
    main , used to test functionality
    @param -
    @return 0 = ok, -1 = error
*/
int main(void)
{
	ljs * my_json = NULL;

	printf("\n");

	printf("[LJS_MAIN] ljs start\n");
	my_json = ljs_init();
	ljs_add_string(my_json,"person:ljsType_object/firstname:ljsType_string","Georg");
	ljs_print(my_json,ljsFormat_pretty);
	ljs_free(my_json);

/*
	//ljs_add_bool(my_json,"lamp:ljsType_bool",1);

	//bool lamp=0;
	//ljs_read_bool(my_json, "lamp:ljsType_bool", &lamp);
	//printf("[LJS_MAIN] lamp is %s\n",lamp?"on":"off");

	//ljs_print(my_json,ljsFormat_pretty);
	//ljs_add_bool(my_json,"lamp:ljsType_bool",0);
	//ljs_add_bool(my_json,"ringer:ljsType_bool",0);
	//ljs_print(my_json,ljsFormat_pretty);
	//ljs_add_bool(my_json,"ringer:ljsType_bool",1);
	//ljs_add_string(my_json,"street:ljsType_string","Reeperbahn");
	//ljs_add_null(my_json,"street:ljsType_null");
	//ljs_print(my_json,ljsFormat_pretty);

	//ljs_add_object(my_json,"person:ljsType_object",NULL);
	//ljs_add_string(my_json,"person:ljsType_object/firstname:ljsType_string","Georg");
	//ljs_add_string(my_json,"person:ljsType_object/name:ljsType_string","Schmitz");
	ljs_add_string(my_json,"person:ljsType_object/Adresse:ljsType_object/Strasse:ljsType_string","Ebertsrasse");
	ljs_print_pointers(my_json);
	ljs_print(my_json,ljsFormat_pretty);
	ljs_free(my_json);
/*
	ljs_add_string(my_json,"person:ljsType_object/Konto:ljsType_object/IBAN:ljsType_string","DE12 4020 0003 2205 02");
	ljs_add_number(my_json,"person:ljsType_object/Konto:ljsType_object/Saldo:ljsType_number",5000.66);

	char *street=NULL;
	ljs_read_string(my_json, "person:ljsType_object/Adresse:ljsType_object/Strasse:ljsType_string",&street);
	printf("[LJS_MAIN] street=%s\n",street?street:"--");

	ljs_print(my_json,ljsFormat_pretty);

	ljs* address=NULL;
	ljs_read_object(my_json, "person:ljsType_object/Adresse:ljsType_object",&address);
	ljs_print(address,ljsFormat_pretty);	


	ljs * Konto=NULL;
	ljs_read_object(my_json, "person:ljsType_object/Konto:ljsType_object",&Konto);
	ljs_print(Konto,ljsFormat_pretty);

	ljs_free(Konto);

	//ljs_print_pointers(my_json);
	ljs_print(my_json,ljsFormat_pretty);



	ljs_free(my_json);
*/


	return 0;
}

/**
    create an initialized json object
    @param -
    @return ljs object, or NULL in case init fails
*/
ljs * ljs_init(void)
{
	ljs * ljsRet= NULL;
	ljsRet = malloc(sizeof(ljs));
	if(ljsRet!=NULL)
	{
		ljsRet->type=ljsType_root;
		ljsRet->child=NULL;
		printf("[LJS] ljs instance %p created\n",ljsRet);
		return ljsRet;
	}
	return NULL;
}

/**
    free a ljs object and all it childs, currently only root can be passed
    @param	pointer to ljs object to be freed
    @return	0 = ok, -1 = error
*/
int ljs_free(ljs *js)
{
	if(js)
	{
		ljs_free_object(js,1);
	}
	return 0;
}

/**
    parse a json string into ljs structure
    @param	pointer to ljs object
    @return	0 = ok, -1 = error
*/
int ljs_add_parse(ljs * js, char * in)
{
	return -1;
}

/**
    add bool object to json object
    @param	pointer to ljs the object where element is inserted
    @return	0 = ok, -1 = error
*/
int ljs_add_bool(ljs *js, char * qualifier, bool val)
{
	if(js)
	{
          printf("[LJS] %s %p bool=%d\n",__FUNCTION__,js,val);
	  return (ljs_write(js,qualifier,(void*) (&val)) );
	}
	return -1;
}

/**
    add null object to json object
    @param	pointer to ljs the object where element is inserted
    @return	0 = ok, -1 = error
*/
int ljs_add_null(ljs *js, char * qualifier)
{
	if(js)
	{
          printf("[LJS] %s %p \n",__FUNCTION__,js);
	  return (ljs_write(js,qualifier,NULL) );
	}
	return -1;
}

/**
    add string object to json object
    @param	pointer to ljs the object where element is inserted
    @return	0 = ok, -1 = error
*/
int ljs_add_string(ljs *js, char * qualifier, char *  val)
{
	if(js)
	{
         printf("[LJS] %s %p string=%s\n",__FUNCTION__,js,val);
	  return (ljs_write(js,qualifier,(char*)val) );
	}
	return -1;
}

/**
    add integer object to json object
    @param	pointer to ljs the object where bool object is inserted
    @return	0 = ok, -1 = error
*/
int ljs_add_number(ljs *js, char * qualifier, double val)
{
	if(js)
	{
          printf("[LJS] %s %p number=%f\n",__FUNCTION__,js,val);
	  return (ljs_write(js,qualifier,(void*) (&val)) );
	}
	return -1;
}


/**
    add null ljs * to json of type object to json object
    @param	pointer to ljs the object where element is inserted
    @return	0 = ok, -1 = error
*/
int ljs_add_object(ljs *js, char * qualifier, ljs* jsAdd)
{
 	if(js)
	{
	  return (ljs_write(js,qualifier,(void*) jsAdd) );
	}
	return -1; 
}

/**
    add null ljs * to json of type array to json object
    @param	pointer to ljs the object where element is inserted
    @return	0 = ok, -1 = error
*/
int ljs_add_array(ljs *js, char * qualifier, ljs* jsAdd)
{
 	if(js)
	{
	  return (ljs_write(js,qualifier,(void*) jsAdd) );
	}
	return -1; 
}

/**
    print ljs object
    @param	pointer to ljs the object where element is inserted
    @return	0 = ok, -1 = error
*/
char * ljs_print(ljs * js, ljsFormat format)
{
	ljs_print_start();
	ljs_print_element(js,format);
	ljs_print_end();
	return NULL;
}


/**
    ...
    @param	...
    @return	...
*/
int   ljs_read_bool(ljs * js, char * qualifier, bool * result)
{
	bool * tmp=NULL;
	int res=-1;
	if(js)
	{
		res=ljs_read(js,qualifier, (void**) &tmp);
		*result=*tmp;
	}
	return -1;
}

/**
    ...
    @param	...
    @return	...
*/
int  ljs_read_null(ljs * js, char * qualifier)
{
	void * tmp=NULL;
	if(js)
	{
		return (ljs_read(js,qualifier, &tmp));
	}
	return -1;
}

/**
    ...
    @param	...
    @return	...
*/
int  ljs_read_string(ljs * js, char * qualifier, char ** result)
{
	if(js)
	{
		return (ljs_read(js,qualifier, (void**) result));
	}
	return -1;
}


/**
    ...
    @param	...
    @return	...
*/
int ljs_read_number(ljs * js, char * qualifier, double * result)
{
	double * tmp=NULL;
	int res=-1;
	if(js)
	{
		res=ljs_read(js,qualifier, (void**) &tmp);
		*result=*tmp;
	}
	return -1;
}

/**
    ...
    @param	...
    @return	...
*/
int  ljs_read_object(ljs * js, char * qualifier, ljs ** result)
{
	if(js)
	{
		return (ljs_read(js,qualifier, (void**) result));
	}
	return -1;
}

/**
    ...
    @param	...
    @return	...
*/
ljs *  ljs_read_get_ref(ljs * js, char * qualifier)
{
	if(js)
	{
		return (ljs_read_ref(js,qualifier));
	}
	return NULL;
}

ljsType ljs_read_get_parent_type(ljs *js)
{
	
	printf("[LJS_WRITE] %s parent check of %p is parent an array\n",__FUNCTION__,js);
	
	while(js)
	{
		if (js->type==ljsType_root)
		{
			printf("[LJS_WRITE] %s root %p\n",__FUNCTION__,js);
			if(js->prev)
			{
				printf("[LJS_WRITE] %s data type=%d\n",__FUNCTION__,js->prev->type);
				return js->prev->type;
			}
		}
		js=js->prev;
	}
	return 0;
}

