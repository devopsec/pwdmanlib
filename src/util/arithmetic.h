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

#ifndef PWDMANLIB_ARITHMETIC_H
#define PWDMANLIB_ARITHMETIC_H

static unsigned long isPrime(unsigned long val) {
    int i, p, exp, a;

    for (i = 9; i--;)
    {
        a = (rand() % (val-4)) + 2;
        p = 1;
        exp = val-1;
        while (exp) {
            if (exp & 1)
                p = (p*a)%val;

            a = (a*a)%val;
            exp >>= 1;
        }

        if (p != 1) {
            return 0;
        }
    }

    return 1;
}

static int findPrimeGreaterThan(int val) {
    if (val & 1) {
        val += 2;
    }
    else {
        val++;
    }

    while (!isPrime(val)) {
        val += 2;
    }
    return val;
}

#endif //PWDMANLIB_ARITHMETIC_H