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

#ifndef PWDMANLIB_RESPONSE_H
#define PWDMANLIB_RESPONSE_H

/********************************************************************************
 *                            Server HTTP Responses                             *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int *getResponseLen(char *response);
static char *createReponse(char *resp_template, char *arg);

/** HTTP response 200, and header for a successful request. **/
static char *ok_response =
        "HTTP/1.0 200 OK\r\n" \
        "Content-type: text/html\r\n" \
        "\r\n\0";

/** HTTP response 400, header, and body */
static char *bad_request_response =
        "HTTP/1.0 400 Bad Request\r\n" \
        "Content-type: text/html\r\n" \
        "\r\n" \
        "<html>\r\n" \
        " <body>\r\n" \
        "  <h1>Bad Request</h1>\r\n" \
        "  <p>This server did not understand your request.</p>\r\n" \
        " </body>\r\n" \
        "</html>\r\n\0";

/** HTTP response 404, header, and body template **/
static char *not_found_response_template =
        "HTTP/1.0 404 Not Found\r\n" \
        "Content-type: text/html\r\n" \
        "\r\n" \
        "<html>\r\n" \
        " <body>\r\n" \
        "  <h1>Not Found</h1>\r\n" \
        "  <p>The requested URL %s was not found on this server.</p>\r\n" \
        " </body>\r\n" \
        "</html>\r\n\0";

/** HTTP response 500, header, and body template **/
static char *server_error_response_template =
        "HTTP/1.0 500 Internal Server Error\r\n" \
        "Content-type: text/html\r\n" \
        "\r\n" \
        "<html>\r\n" \
        " <body>\r\n" \
        "  <h1>Internal Server Error</h1>\r\n" \
        "  <p>The server encountered an error and cannot process your request.</p>\r\n" \
        "  <p>Error: %s.\r\n</p>" \
        " </body>\r\n" \
        "</html>\r\n\0";

/** HTTP response 501, header, and body template **/
static char *bad_method_response_template =
        "HTTP/1.0 501 Method Not Implemented\r\n" \
        "Content-type: text/html\r\n" \
        "\r\n" \
        "<html>\r\n" \
        " <body>\r\n" \
        "  <h1>Method Not Implemented</h1>\r\n" \
        "  <p>The method %s is not implemented by this server.</p>\r\n" \
        " </body>\r\n" \
        "</html>\r\n\0";

/** return a ptr to length of the response (NOT AN ARRAY) **/
static int *getResponseLen(char *response) {
    int *len = (int*) malloc(sizeof(int*));            // allocate
    *len = (int) strlen(response);                     // set val
    return len;
}

/** return a formatted and allocated response str, created from template   **
 ** arg is the str argument inserted into the response template            **
 ** note: arg must be a null terminated string (null char will be removed) **/
static char *createReponse(char *resp_template, char *arg) {
    char *resp;
    size_t len = strlen(resp_template) + strlen(arg);
    resp = (char*) malloc(len);
    snprintf(resp, len, resp_template, arg);
    return resp;
}

#endif //PWDMANLIB_RESPONSE_H
