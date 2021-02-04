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
#include "libjson_memory.h"

#define _LIBJSON_MEM_DBG_    0
#if _LIBJSON_MEM_DBG_
    #define LIBJSON_MEM_DBG(x) x
#else
    #define LIBJSON_MEM_DBG(x)
#endif
void * libjson_malloc(int size)
{
    void * mem=malloc(size);
    memset(mem,0,size);
    LIBJSON_MEM_DBG(printf("[LJS_MEM] %s p=%p size=%d\n",__FUNCTION__,mem,size);)
    return mem;
}
void libjson_free(void * mem)
{
    LIBJSON_MEM_DBG(printf("[LJS_MEM] %s p=%p\n",__FUNCTION__,mem);)
    free(mem);
}