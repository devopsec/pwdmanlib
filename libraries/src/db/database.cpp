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
#include <libpq-fe.h>

void shutdown_server(PGconn *db_conn) {
    PQfinish(db_conn);
    exit(1);
}

int main() {
    PGconn *db_conn = PQconnectdb("user=tmp password=pass dbname=testdb");

    int lib_ver = PQlibVersion(db_conn);
    printf("PostgreSQL DB version: %d\n", lib_ver);

    /* check for connection errors */
    if (PQstatus(db_conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(db_conn));
        shutdown_server(db_conn);
    }

    /* debug show user info */
    char *user = PQuser(db_conn);
    char *db_name = PQdb(db_conn);
    char *pswd = PQpass(db_conn);

    printf("User: %s\n", user);
    printf("Database name: %s\n", db_name);
    printf("Password: %s\n", pswd);

    PQfinish(db_conn);
    return 0;
}
