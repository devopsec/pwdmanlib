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

/********************************************************************************
 *                    Demonstrate usage of vector arrays                        *
 ********************************************************************************/

#include <stdio.h>
#include "util/vector.h"

int main() {  /*    dynamically-sizing vector         */
    int i;
    Vector vector;                  /* declare a new vector      */
    int_vector_init(&vector, 1000); /* initialize the new vector */

    /* fill it up with 150 arbitrary values
    this should expand capacity up to 200 */
    for (i = 200; i > -50; i--) {
        int_vector_append(&vector, i);
    }

    /* set a value at an arbitrary index
    this will expand and zero-fill the vector to fit */
    int_vector_set(&vector, 4452, 21312984);

    /* print out an arbitrary value in the vector */
    printf("Heres the value at 27: %d\n", int_vector_get(&vector, 27));

    /* free its underlying data array */
    int_vector_free(&vector);

    /*    dynamically incrementing vector    */
}