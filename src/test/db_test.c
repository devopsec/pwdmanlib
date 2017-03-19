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

#include "db/database.h"

int main() {
    int status = createTable();
    const char *user1 = "foo";
    const char *user2 = "foofoo";
    const char *user3 = "TheFooestF00";
    const char *pw1 = "bar";
    const char *pw2 = "extrabar";
    const char *pw3 = "H1gh3stBar";
    const char *wrongpw = "wrong";

    if (status != 0) {
        return -1;
    }

    char *ver = getVersion();
    if (strcmp(ver, "FAIL") == 0) {
        return -1;
    }

    char **table = getTables();
    if (strcmp(table[0], "FAIL") == 0) {
        return -1;
    } if (table) { free(table); }

    UserRow *rows = getTableRows();
    if (strcmp(rows->user, "FAIL") == 0) {
        return -1;
    } //if (rows) { free(rows); }

    if (authenticateUser(user1, pw1) != 0) {
        return -1;
    }
    if (authenticateUser(user2, pw2) != 0) {
        return -1;
    }
    if (authenticateUser(user3, pw3) != 0) {
        return -1;
    }

    if (authenticateUser(user1, wrongpw) == 0) {
        return -1;
    }

    if (authenticateUser(user2, wrongpw) == 0) {
        return -1;
    }

    if (authenticateUser(user3, wrongpw) == 0) {
        return -1;
    }
    return 0;
}