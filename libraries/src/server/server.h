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

#ifndef PWDMANLIB_SERVER_H
#define PWDMANLIB_SERVER_H

/********************************************************************************
 *               Encapsulation of sensitive / shared data                       *
 ********************************************************************************/


/********************************************************************************
 *             HTTP Protocol Definitions                (RFC 822)               *
 ********************************************************************************/

#include "util/oop.h"
#include "util/stringops.h"
#include "util/constants.h"
#include "server/fawkes_proto.h"

typedef struct request Request;
struct request {
    unsigned char    info;        /*   request method / proto */
    char            *path;        /*   path to resource       */
    Dict            *headers;     /*   http headers           */
    char            *body;        /*   body of request        */
    Dict            *query;       /*   query key-vals         */
};

int sendall(int fd, char *buf, ssize_t *len);
char **parseHeader(char *str);
char **parseQueryStr(char *str, size_t *num_qtokens);
void routeClient(Request req, int worker_sock, char *root_dir);

/** parse request line from a client request **/
Request parseRequest(char *req) {
    int i, j;

    printf("processing client request\n");
    size_t *num_tokens = malloc(sizeof(size_t));             /* num of tokens */
    size_t *num_sub_tokens = malloc(sizeof(size_t));         /* num of tokens */
    size_t *num_query_tokens = malloc(sizeof(size_t));       /* num of tokens */

    char **tokens = strsplit(req, "\r\n", num_tokens);     /* parse it */
    printf(ANSI_COLOR_CYAN  "num_tokens: %zi\n"  ANSI_COLOR_RESET, *num_tokens); //DEBUG

    for (i = 0; i < *num_tokens; i++) { //DEBUG
        printf(ANSI_COLOR_CYAN  "token: %s\n"  ANSI_COLOR_RESET, tokens[i]);
    }

    char **sub_tokens = strsplit(tokens[0], " ", num_sub_tokens);     /* parse it */
    printf(ANSI_COLOR_CYAN  "num_sub_tokens: %zi\n"  ANSI_COLOR_RESET, *num_sub_tokens); //DEBUG

    for (i = 0; i < *num_sub_tokens; i++) {
        printf(ANSI_COLOR_CYAN  "subtoken: %s\n"  ANSI_COLOR_RESET, sub_tokens[i]); //DEBUG
    }

    Request retreq = {
            .info = 0,                                                            /* method, proto, validity  */
            .path = (char *) malloc(sizeof(char*)),                               /*    request path or URI   */
            .headers = initDict(50),                                              /*    limit to 50 headers   */
            .body = (char *) malloc(sizeof(char*)),                               /*      body of request     */
            .query = initDict(20)                                                 /* limit to 20 query params */
    };

    if (!strchr(sub_tokens[1], '?')) {                                            /* check for query string   */
        strcpy(retreq.path, sub_tokens[1]);                                       /* set path */
    } else {
        char **query_tokens = parseQueryStr(sub_tokens[1], num_query_tokens);
        if (*num_query_tokens > 41) {              /* they are split between 2 tokens each and first is path  */
            /* response 400 */
        } strcpy(retreq.path, query_tokens[0]);                                   /* set path */

        for (i = 1; i < *num_query_tokens; i += 2) {
            char *key = query_tokens[i];
            char *val = query_tokens[i+1];
            setTuple(retreq.query, key, val);
        }

        for (i = 0; i < 2; i++) { // DEBUG
            printf(ANSI_COLOR_CYAN  "QUERY token (key): %s\n"  ANSI_COLOR_RESET, query_tokens[i]);
            printf(ANSI_COLOR_CYAN  "QUERY token (val): %s\n"  ANSI_COLOR_RESET, query_tokens[i+1]);
        }
//        if (query_tokens) free(query_tokens);    //ERROR causing SIGV
    }

    for (i = 1; i < *num_tokens - 2; i++) {                                      /* set the headers       */
        printf("setting header %i\n", i);
        if (tokens[i] == "") { i--; continue; }                                  /* get rid of empty line */
        char **header = parseHeader(tokens[i]);                                  /* parse um              */
        if (!header[1]) { i--; if (header) free(header); continue; }             /* get rid of empty body */
        setTuple(retreq.headers, header[0], header[1]);                          /* set the tuple in dict */

        for (j = 0; j < 2; j++) { // DEBUG
            printf(ANSI_COLOR_CYAN  "HEADER token: %s\n"  ANSI_COLOR_RESET, header[j]);
        }
        if (header) free(header);
    }

    for (i = 0; i < *num_tokens; i++) {                                              /* search for empty string  */
        if (tokens[i] && !strlen(tokens[i])) {                                       /* ensure body was provided */
            if (tokens[i+1] != NULL) { strcpy(retreq.body, tokens[i+1]); break; }    /* grab body (next token)   */
            else {retreq.body = '\0'; break; }                                         /* null out body and break  */
        }
    }

    printByteDebug(retreq.info);  //DEBUG
    retreq.info = encodeProto(sub_tokens[2], retreq.info);
    printByteDebug(retreq.info);  //DEBUG
    retreq.info = encodeMethod(sub_tokens[0], retreq.info);
    printByteDebug(retreq.info);  //DEBUG
    retreq.info = encodeReqmts(retreq.info);
    printByteDebug(retreq.info);  //DEBUG

//    if(tokens) free(tokens);            //ERROR receiving SIGABRT
//    if (sub_tokens) free(sub_tokens);   //ERROR receiving SIGABRT
    if (num_tokens) free(num_tokens);
    if (num_sub_tokens) free(num_sub_tokens);
    if (num_query_tokens) free(num_query_tokens);

    printf("client request processed\n");
    return retreq;
}

char **parseQueryStr(char *str, size_t *num_qtokens) {
    int i = 0, j = 1;
    size_t *count = malloc(sizeof(size_t));
    char *tok, *sub_str;
    char **tokens = strsplit(str, "?", count);
    char **sub_tokens = strsplit(tokens[1], "&", count);

    char **result = '\0';
    result = (char**) malloc(sizeof(char*) * ((*count * 2) + 1));
    result[0] = strdup(tokens[0]);        /* path */

    if (result) {
        for (i = 0; i < *count; i++) {
            sub_str = sub_tokens[i];
            char save[strlen(sub_str)];

            if (save) {
                if ((tok = strstr(sub_str, "=")) != NULL) {
                    strncpy(save, sub_str, (tok - sub_str));
                    save[(tok - sub_str)] = '\0';
//                    printf("Token extracted: <<%s>>\n", save);
                    result[j++] = strdup(save);
                    tok += 1;
                    sub_str = tok;
                } /* grab trailing token */

                if (sub_str && sub_str[0] != '\0') {
                    result[j++] = strdup(sub_str);
//                    printf("Token extracted: <<%s>>\n", sub_str);
                }
            }
            else { *num_qtokens = 0; break; }
        } result[j] = '\0';
        *num_qtokens = (size_t) j;
    } else {
        *num_qtokens = 0;
    }

//    if (tok) free(tok);                  //ERROR causing SIGV
//    if (sub_str) free(sub_str);          //ERROR causing SIGV
//    if (sub_tokens) free(sub_tokens);    //ERROR causing SIGV
    if (count) free(count);
    if (tokens) free(tokens);

    return result;
}

char **parseHeader(char *str) {
    char *save, *tok;              /* holds str_tok val btwn calls */
    char **result = '\0';          /* set result to NULL */
    char *tmp = strdup(str);       /* leaves original str intact */
    size_t delims_size = strlen(": ");
    int i = 0;

    save = malloc(sizeof(char) * strlen(str));
    result = malloc(sizeof(char *) * 2);

    if ((tok = strstr(tmp, ": ")) != NULL) {
        strncpy(save, tmp, (tok - tmp));
        save[(tok - tmp)] = '\0';
        result[i++] = strdup(save);
        tok += delims_size;
        tmp = tok;
    } /* grab value token */
    if (tmp != "" || tmp != NULL) {
        result[i++] = strdup(tmp);
    }
    if (save) free(save);
    return result;
}

/** route client to view based on parsed data and credentials **/
void routeClient(Request req, int worker_sock, char *root_dir) {
    char *resp_data = '\0', *p;
    char path[PATH_MAX] = {'\0'};
    short method;

    if ((decodeReqmts(req.info)) == -1) {
        write(worker_sock, "HTTP/1.1 400 Bad Request\r\n", 27);
    }
    if ((decodeProto(req.info)) == -1) {
        write(worker_sock, "HTTP/1.1 400 Bad Request\r\n", 27);
    }
    if ((method = decodeMethod(req.info)) == -1) {
        write(worker_sock, "HTTP/1.1 400 Bad Request\r\n", 27);
    }

    switch (method) {
        case 1:
            /* If no file specified, index.html will be opened by default */
            if (strncmp(req.path, "/\0", 2) == 0) {
                req.path = "/index.html";
            }

            strcpy(path, root_dir);                         /* copy root dir to path variable */
            strcpy(&path[strlen(root_dir)], req.path);      /* append file to get to root dir */
            path[strlen(path)] = '\0';                      /* null terminate path string     */
            printf("resource path: %s\n", path);

            //DEBUG
            printf(ANSI_COLOR_BLUE "File: %s\nFunction: %s()\nLine Number: %d\n" ANSI_COLOR_RESET, __FILE__, __FUNCTION__, __LINE__);

            resp_data = readFile(path);
            if (strtol(resp_data, &p, 0) == -1) { /* FILE NOT FOUND */
                write(worker_sock, "HTTP/1.1 404 Not Found\r\n", 24);
            }
            else { /* FILE FOUND */
                ssize_t *fileLen = (ssize_t*) malloc(sizeof(ssize_t*));
                *fileLen = strlen(resp_data);

                if (sendall(worker_sock, resp_data, fileLen) == -1) {
                    send(worker_sock, "HTTP/1.1 200 OK\r\n", 17, 0);
                }
                send(worker_sock, "HTTP/1.1 500 Server Error\r\n", 27, 0);
            }

        default:
            printf("Could not method client to resource");
            write(worker_sock, "HTTP/1.1 500 Server Error\r\n", 27);

    }
}

/* ensure kernel doesn't hold onto our data */
int sendall(int fd, char *buf, ssize_t *len) {
    int total = 0;            // how many bytes we've sent
    ssize_t bytesleft = *len; // how many we have left to send
    ssize_t n;

    while(total < *len) {
        n = send(fd, buf + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total;            // return number actually sent here
    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}


/*=======================  More Settings for Headers ==========================*/
/*-----------------------        Request Headers     --------------------------*/
//  Authorization: <auth-type> //specify auth type
//  Accept-Charset: utf-8\n //specify character type
//  Accept-Encoding: gzip, deflate, sdch\n //specify encoding
//  Accept: text/html,application/xhtml+xml,application/xml //for xml
//  Accept: application/json, text/javascript // for api's
//  Connection: keep-alive //keep connection alive
//  X-AUTH-TOKEN: <auth hash> //for token auth
//  Cookie: <cookie hash> //provide cookie
//  User-Agent: Mozilla/5.0 //provide user agent
//  Accept-Language: en-US, en\n //ask for language
//  Cache-Control: no-cache\n //set this for login pages
//  Upgrade-Insecure-Requests: 1\n //set this for login pages
//  Content-Type: application/x-www-form-urlencoded //set MIME type for forms
//  Host: <domain name>:<port> //domainname of the server
//  If-Unmodified-Since: <timestamp> //only dispatch request if unmodified
//  Proxy-Authorization: Basic <hash> //auth creds for connecting to proxy
//  X-Csrf-Token: <hash> // used to prevent cross-site request forgery
//  Referer: <url> //url of page or link that sent user here
/*-----------------------       Response Headers      -------------------------*/
//  Access-Control-Allow-Origin: * //set which sites can share cross-site resources
//  Age: 12 //age object has been in proxy cache in seconds
//  Allow: GET, HEAD //set what request methods are allowed
//  Cache-Control: max-age=3600 //set to 0 on login pages
//  Connection: close //whether to keep connection alive
//  Content-Disposition: attachment; filename="fname.ext" //for hosting files
//  Content-Encoding: gzip
//  Content-Language: en
//  Content-Type: text/html; charset=utf-8
//  Date: Tue, 15 Nov 1994 08:12:31 GMT
//  Expires: Thu, 01 Dec 1994 16:00:00 GMT
//  Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
//  Location: http://www.w3.org/pub/WWW/People.html //used for redirection
//  Proxy-Authenticate: Basic //set proxy auth
//  Retry-After: 120 //retry request after, in seconds
//  Server: Apache/2.4.1 (Unix) //name for the server
//  Set-Cookie: UserID=JohnDoe; Max-Age=3600; Version=1 //set cookie
//  Status: 200 OK //status of response
//  Strict-Transport-Security: max-age=16070400; includeSubDomains
//  Transfer-Encoding: chunked //encoding used to transfer data to user
//  Upgrade: HTTP/2.0, HTTPS/1.3, IRC/6.9, RTA/x11, websocket //ask client to upgrade to another proto
//  WWW-Authenticate: Basic //set auth theme
//  X-XSS-Protection: 1; mode=block //cross-site scripting (XSS) filter
//  X-WebKit-CSP: default-src 'self' //content security policy
//  X-Content-Type-Options: nosniff //prevent MIME-sniffing
//  X-Powered-By: PHP/5.4.0 //specify serving technology
//  X-UA-Compatible: IE=EmulateIE7 //recommend a preferred rendering engine
//  X-UA-Compatible: IE=edge //recommend a preferred rendering engine
//  X-UA-Compatible: Chrome=1 //recommend a preferred rendering engine
//  Upgrade-Insecure-Requests: 1 //tells client which hoists will handle redirection to https
//  X-Request-ID: <hash id> //correlate HTTP request between client and server

#endif //PWDMANLIB_SERVER_H
