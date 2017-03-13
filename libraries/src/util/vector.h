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
 *               A dynamically sizing integer vector array                      *
 ********************************************************************************/

typedef struct {
    size_t size;      // slots used so far
    size_t capacity;  // total available slots
    int *data;        // integer array
} Vector;

void int_vector_init(Vector *vector,  size_t vector_initial_capacity);

void int_vector_append(Vector *vector, int value);

int  int_vector_get(Vector *vector, int index);

void int_vector_set(Vector *vector, int index, int value);

void int_vector_resize(Vector *vector);

void int_vector_free(Vector *vector);

/********************************************************************************
 *                A dynamically incrementing vector array                       *
 ********************************************************************************/

/* override the constant locally to change */
static size_t const VECTOR_CAPACITY = 1000;

typedef struct {

    size_t size;      // slots used so far
    size_t capacity;  // total available slots
    int *data;     // integer array
} IncrementVector;

void increment_vector_init(IncrementVector *vector, size_t vector_initial_capacity);

void increment_vector_append(IncrementVector *vector, int value);

int increment_vector_get(IncrementVector *vector, int index);

void increment_vector_set(IncrementVector *vector, int index, int value);

void increment_vector_double_capacity_if_full(IncrementVector *vector);

void increment_vector_free(IncrementVector *vector);

#endif //PWDMANLIB_VECTOR_H
