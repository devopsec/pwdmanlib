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

#ifndef PWDMANLIB_VECTOR_H
#define PWDMANLIB_VECTOR_H

/********************************************************************************
 *                   A dynamically sizing vector array                          *
 ********************************************************************************/

/* override the constant locally to change */
static int const VECTOR_INITIAL_CAPACITY = 1000;

typedef struct {
    int size;      // slots used so far
    int capacity;  // total available slots
    int *data;     // integer array
} Vector;

void vector_init(Vector *vector);

void vector_append(Vector *vector, int value);

int vector_get(Vector *vector, int index);

void vector_set(Vector *vector, int index, int value);

void vector_double_capacity_if_full(Vector *vector);

void vector_free(Vector *vector);

/********************************************************************************
 *                A dynamically incrementing vector array                       *
 ********************************************************************************/

/* override the constant locally to change */
static int const VECTOR_CAPACITY = 1000;

typedef struct {
    int size;      // slots used so far
    int capacity;  // total available slots
    int *data;     // integer array
} IncrementVector;

void increment_vector_init(IncrementVector *vector);

void increment_vector_append(IncrementVector *vector, int value);

int increment_vector_get(IncrementVector *vector, int index);

void increment_vector_set(IncrementVector *vector, int index, int value);

void increment_vector_double_capacity_if_full(IncrementVector *vector);

void increment_vector_free(IncrementVector *vector);

#endif //PWDMANLIB_VECTOR_H
