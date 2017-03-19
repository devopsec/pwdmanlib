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

#ifndef PWDMANLIB_DATABASE_H
#define PWDMANLIB_DATABASE_H

/********************************************************************************
 *                         Database Access Functions                            *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "util/constants.h"
#include "util/stringops.h"

char *getVersion();
typedef struct user_row UserRow;
struct user_row {
    char *user;
    char *pass;
};

static short success(PGconn *conn, PGresult *res) {
    PQclear(res);
    PQfinish(conn);
    return(0);

}

static short failInt(PGconn *conn, PGresult *res) {
    PQclear(res);
    PQfinish(conn);
    return(-1);
}

static char *failChar(PGconn *conn, PGresult *res) {
    PQclear(res);
    PQfinish(conn);
    char* msg = "FAIL";
    return(msg);
}

void printDBInfo(PGconn *conn) {
    char *ver = getVersion();
    char *user = PQuser(conn);
    char *db_name = PQdb(conn);
    char *pswd = PQpass(conn);
    printf(ANSI_COLOR_BLUE "User: %s\n", user);
    printf("Database name: %s\n", db_name);
    printf("Password: %s\n" ANSI_COLOR_RESET, pswd);
    printf("Database Version: %s\n", ver);
}

char *getVersion() {
    PGconn *conn = PQconnectdb("user=tmp password=pass dbname=testdb hostaddr=127.0.0.1 port=5432 sslmode=require");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        char *msg = "FAIL";
        return msg;
    }

    PGresult *res = PQexec(conn, "SELECT VERSION()");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");
        failChar(conn, res);
    }

    char *ver = PQgetvalue(res, 0, 0);

    // DEBUG
    printf("Database Version: %s\n", ver);

    PQclear(res);
    PQfinish(conn);
    return ver;
}

char **getTables() {
    int i, num_rows;

    PGconn *conn = PQconnectdb("user=tmp password=pass dbname=testdb hostaddr=127.0.0.1 port=5432 sslmode=require");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        char *msg[1] = {"FAIL"};
        return msg;
    }

    PGresult *res = PQexec(conn, "SELECT table_name FROM information_schema.tables "\
            "WHERE table_schema = 'public'");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");
        PQclear(res);
        PQfinish(conn);
        char *msg[1] = {"FAIL"};
        return msg;
    }

    num_rows = PQntuples(res);
    char **rows = malloc(sizeof(char*) * num_rows);

    for (i = 0; i < num_rows; i++) {
        rows[i] = PQgetvalue(res, i, 0);
        printf("Table #%i: %s\n", i+1, rows[i]); // DEBUG
    }

    PQclear(res);
    PQfinish(conn);
    return rows;
}

UserRow *getTableRows() {
    int i, num_rows;

    PGconn *conn = PQconnectdb("user=tmp password=pass dbname=testdb hostaddr=127.0.0.1 port=5432 sslmode=require");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        UserRow *rows = malloc(sizeof(UserRow*));
        rows[0].user = "FAIL";
        rows[0].pass = "FAIL";
        return rows;
    }

    PGresult *res = PQexec(conn, "SELECT * FROM Users");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");
        PQclear(res);
        PQfinish(conn);
        UserRow *rows = malloc(sizeof(UserRow*));
        rows[0].user = "FAIL";
        rows[0].pass = "FAIL";
        return rows;
    }

    num_rows = PQntuples(res);
    UserRow *rows = malloc(sizeof(UserRow*) * num_rows);

    for (i = 0; i < num_rows; i++) {
        rows[i].user = PQgetvalue(res, i, 0);
        rows[i].pass = PQgetvalue(res, i, 1);
        printf("Row   #%i: %s    %s\n", i+1, rows[i].user, rows[i].pass); // DEBUG
    }

    PQclear(res);
//    PQfinish(conn);
    return rows;
}

/* TODO: add support for creating db from file */
int createTable() {
    const char *conninfo = "user=tmp password=pass dbname=testdb hostaddr=127.0.0.1 port=5432 sslmode=require";
    PGconn *conn = PQconnectdb(conninfo);   /* connect to db */
    PGresult *res;
    FILE data;
    int num_fields;
    int i, j;

    // Make a connection to the database
    conn = PQconnectdb(conninfo);

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    printDBInfo(conn); // DEBUG

    /* drop table if exists */
    res = PQexec(conn, "DROP TABLE IF EXISTS Users");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        return failInt(conn, res);
    }
    PQclear(res);

    /* create table */
    res = PQexec(conn, "CREATE TABLE Users(username VARCHAR(20) PRIMARY KEY," \
        "password VARCHAR(45))");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        return failInt(conn, res);
    }
    PQclear(res);

    /* add some users */
    res = PQexec(conn, "INSERT INTO Users (username,password) VALUES ('foo','bar')");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "INSERT failed: %s", PQerrorMessage(conn));
        return failInt(conn, res);
    }
    PQclear(res);

    res = PQexec(conn, "INSERT INTO Users (username,password) VALUES ('foofoo','extrabar')");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "INSERT failed: %s", PQerrorMessage(conn));
        return failInt(conn, res);
    }
    PQclear(res);

    res = PQexec(conn, "INSERT INTO Users (username,password) VALUES ('TheFooestF00','H1gh3stBar')");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "INSERT failed: %s", PQerrorMessage(conn));
        return failInt(conn, res);
    }
    PQclear(res);

//    res = PQexec(conn, "COMMIT");
//    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//        printf("COMMIT command failed\n");
//        failInt(res, conn);
//    }
//    PQclear(res);

    PQfinish(conn); /* close the connection */
    return 0;
}

/* TODO: whitelisting / parsing / verifying user and passw, and add error returning */
/** Returns 0 on success, Returns -1 on Failure **/
int authenticateUser(const char *user, const char *pass) {
    const char *statement = "SELECT password FROM Users WHERE username='";
    size_t query_size = strlen(statement) + strlen(user) + 3;
    char *query = malloc(query_size);
    memset(query, '\0', query_size);

    PGconn *conn = PQconnectdb("user=tmp password=pass dbname=testdb hostaddr=127.0.0.1 port=5432 sslmode=require");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    strcat(query, statement);
    strcat(query, user);
    strcat(query, "';\0");
    printf("query: %s\n",query);

    PGresult *res = PQexec(conn, query);
    printf("num of tuples: %i\n", PQntuples(res));
    printf("num of columns: %i\n", PQnfields(res));
//    PQprintTuples(res, STDOUT_FILENO, )

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("No data retrieved\n");
        return failInt(conn, res);
    }

    char *pass_check = PQgetvalue(res, 0, 0);
    if (strcmp(pass, pass_check) == 0) {
        return success(conn, res);
    }

    PQclear(res);
    PQfinish(conn);
    return -1;
}

#endif //PWDMANLIB_DATABASE_H
