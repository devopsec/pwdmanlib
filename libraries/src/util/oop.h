/********************************************************************************
 *                          Copyright 2017 DevOpSec                             *
 *                                                                              *
 *   Licensed under the Apache License, Version 2.0 (the "License");            *
 *   you may not use this file except in compliance with the License.           *
 *   You may obtain a copy of the License at                                    *
 *                                                                              *
 *       http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                              *
 *   Unless required by applicable law or agreed to in writing, software        *
 *   distributed under the License is distributed on an "AS IS" BASIS,          *
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 *   See the License for the specific language governing permissions and        *
 *   limitations under the License.                                             *
 *                                                                              *
 *      Contributors: ....                                                      *
 *                                                                              *
 ********************************************************************************/

#ifndef PWDMANLIB_OOP_H
#define PWDMANLIB_OOP_H
#define EMPTY_STRING {'\0'}

/********************************************************************************
 *             Utility classes and functions for OOP operations                 *
 ********************************************************************************/

#include <stdio.h>

/******************               HASH MAP               ************************/
// pre-declarations of structured types to key-val map
typedef struct hash_map *hash_map_t;
// key-value pair checking
typedef int (*hash_map_validator_t)(const void *key1, const void *key2, void *data);
// a "constructor"
hash_map_t create_map (hash_map_validator_t compare_keys, void *data);
// a "destructor"
void destroy_map (hash_map_t map);
// setter
void set_map (hash_map_t map, const void *key, const void *value);
// getter
void *get_map (hash_map_t map, void *key);
// de-referencing
void *remove_map (hash_map_t map, void *key);

/*******************           Virtual Class             ************************/

// TODO: sort through this and see if there's anything useful

// The top-level class.

typedef struct _tCommClass {
    int (*open)(struct _tCommClass *self, char *fspec);
} tCommClass;

// Function for the TCP 'class'.

static int tcpOpen (tCommClass *tcp, char *fspec) {
    printf ("Opening TCP: %s\n", fspec);
    return 0;
}
static int tcpInit (tCommClass *tcp) {
    tcp->open = &tcpOpen;
    return 0;
}

// Function for the HTTP 'class'.

static int httpOpen (tCommClass *http, char *fspec) {
    printf ("Opening HTTP: %s\n", fspec);
    return 0;
}
static int httpInit (tCommClass *http) {
    http->open = &httpOpen;
    return 0;
}

/*******************              Modules                ************************/

typedef struct Module_struct Module;          /* module declaration */

Module *Module_create();                      /* constructor */
void    Module_delete( Module * );            /* destructor */
int     Module_get_element( const Module * ); /* accessor */
void    Module_set_element( Module *, int );  /* modifier */


#endif //PWDMANLIB_OOP_H