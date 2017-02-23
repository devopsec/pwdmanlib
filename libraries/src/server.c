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
 *                  Simple Web Server for Hosting GUI                           *
 *                  cmd line opts: -p [port] -r [path]                          *
 ********************************************************************************/

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CONNMAX 1000
#define BYTES   1024

char *ROOT;
int listenfd, clients[CONNMAX];
void error(char *);
void startServer(char *);
void respond(int);

int main() {
    int create_sock, new_sock;
    socklen_t address_len;
    struct sockaddr_in address;

    size_t bufsize = BUFSIZ;
    char *buffer = (char*) malloc(bufsize);

    if ((create_sock = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("Socket has been created\n");
    }

    /* address / routing settingsd */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9630);

    if (bind(create_sock, (struct sockaddr*) &address, sizeof(address)) == 0) {
        printf("Binding to socket\n");
    }

    while (1) {
        if (listen(create_sock, 10) < 0) {
            perror("Error listening to socket:");
            exit(1);
        }

        if ((new_sock = accept(create_sock, (struct sockaddr*) &address, &address_len)) < 0) {
            perror("Error accepting traffic on socket:");
            exit(1);
        }

        if (new_sock > 0) {
            printf("Client connection established!\n");
        }

        /* write buffer to new sock */
        recv(new_sock, buffer, bufsize, 0);
        printf("%s\n", buffer);
        /* write RESPONSE headers to sock */
        write(new_sock, "HTTP/2.0 200 OK\n", 16);
        write(new_sock, "Content-length: 46\n", 19);
        write(new_sock, "Content-Type: text/html\n", 26);
        write(new_sock, "Accept-Ranges: bytes\n", 21);
        write(new_sock, "Connection: close\n", 18);
        write(new_sock, "Content-Type: text/html; charset=UTF-8\n", 39);
        write(new_sock, "Content-Encoding: gzip,deflate\n", 31);
        write(new_sock, "<html><body><H1>Welcome to the PwdManLib dashboard!</H1></body></html>", 70);
        close(new_sock);
    }
    close(create_sock);
    free(buffer);
    return 0;
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


/*
//  Now you can extend this with other functions such as:
//  Serving files (you can use sendfile() function)
//  Serving different content types(images, zip, pdf etc)
//  Different processes to handle requests (fork())
//  Other http status codes
//  Processing PHP (process PHP with system commands and write the result to the socket)
 */