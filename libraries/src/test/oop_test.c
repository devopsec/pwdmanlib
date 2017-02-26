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
 *               Testing for Utility classes and OOP functions                  *
 ********************************************************************************/

#include <stdlib.h>
#include "util/oop.h"

/*******************           Virtual Class             ************************/

int main (void) {
    int status;
    tCommClass commTcp, commHttp;

    // Same 'base' class but initialised to different sub-classes.

    tcpInit(&commTcp);
    httpInit(&commHttp);

    // Called in exactly the same manner.

    status = (commTcp.open)(&commTcp, "bigiron.box.com:5000");
    status = (commHttp.open)(&commHttp, "http://www.microsoft.com");




/*******************              Modules                ************************/

   /* include type declaration after structure definition */

    Module *Module_create() { return (Module *) calloc(1, sizeof(Module)); }
    void Module_delete(Module *m) { free(m); }
    int Module_get_element(const Module *m) { return m->element; }
    void Module_set_element(Module *m, int e) { m->element = e; }

    return 0;
}