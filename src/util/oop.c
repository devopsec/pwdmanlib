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

#include <stdlib.h>
#include "oop.h"

/******************               HASH MAP               ************************/

typedef struct {
    unsigned long           size, count;
    hash_map_t              *entries;
    void                    *data;
} hash_map;
//typedef struct hash_map *hash_map_t;
// key-value pair checking
typedef int (*hash_map_validator_t)(const void *key1, const void *key2, void *data);

/* constructor */
hash_map_t create_map (hash_map_validator_t compare_keys, void *data) {

}
// destructor
void destroy_map (hash_map_t map);
// setter
void set_map (hash_map_t map, const void *key, const void *value);
// getter
void *get_map (hash_map_t map, void *key);
// de-referencing
void *remove_map (hash_map_t map, void *key);

