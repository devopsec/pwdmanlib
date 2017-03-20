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

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

/********************************************************************************
 *               A dynamically sizing integer vector array                      *
 ********************************************************************************/

/* define initial capacity during initialization */
void int_vector_init(Vector *vector, size_t vector_initial_capacity) {
    // initialize size and capacity
    vector->size = 0;
    vector->capacity = vector_initial_capacity;

    // allocate memory for vector->data
    vector->data = malloc(sizeof(int) * vector->capacity);
}

void int_vector_append(Vector *vector, int value) {
    // make sure there's room to expand into
    int_vector_resize(vector);

    // append the value and increment vector->size
    vector->data[vector->size++] = value;
}

int int_vector_get(Vector *vector, int index) {
    if (index >= vector->size || index < 0) {
        printf("Index %d out of bounds for vector of size %d\n", index, (int) vector->size);
        exit(1);
    }
    return vector->data[index];
}

void int_vector_set(Vector *vector, int index, int value) {
    // zero fill the vector up to the desired index
    while (index >= vector->size) {
        int_vector_append(vector, 0);
    }

    // set the value at the desired index
    vector->data[index] = value;
}

void int_vector_resize(Vector *vector) {
    if (vector->size >= vector->capacity) {
        // double vector->capacity and resize the allocated memory accordingly
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
    }
}

void int_vector_free(Vector *vector) {
    free(vector->data);
}

/********************************************************************************
 *                A dynamically incrementing vector array                       *
 ********************************************************************************/
// TODO: need to figure this out and implement in server.c
void increment_vector_init(IncrementVector *vector, size_t vector_initial_capacity) {
    // initialize size and capacity
    vector->size = 0;
    vector->capacity = vector_initial_capacity;

    // allocate memory for vector->data
    vector->data = malloc(sizeof(int) * vector->capacity);
}

void increment_vector_append(IncrementVector *vector, int value) {
    // make sure there's room to expand into
    increment_vector_double_capacity_if_full(vector);

    // append the value and increment vector->size
    vector->data[vector->size++] = value;
}

int increment_vector_get(IncrementVector *vector, int index) {
    if (index >= vector->size || index < 0) {
        printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
        exit(1);
    }
    return vector->data[index];
}

void increment_vector_set(IncrementVector *vector, int index, int value) {
    // zero fill the vector up to the desired index
    while (index >= vector->size) {
        increment_vector_append(vector, 0);
    }

    // set the value at the desired index
    vector->data[index] = value;
}

void increment_vector_double_capacity_if_full(IncrementVector *vector) {
    if (vector->size >= vector->capacity) {
        // double vector->capacity and resize the allocated memory accordingly
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
    }
}

void increment_vector_free(IncrementVector *vector) {
    free(vector->data);
}