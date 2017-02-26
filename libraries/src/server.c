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

#define CONNMAX     1000
#define SOCKBUFF    1024

/* function declarations */
int start_server(char *);
void respond(int);

/* DEBUG: TEMP global vars */
int clients[CONNMAX];
char *root_dir;

int main(int argc, char* argv[]) {
    char *root_dir = getenv("PWD");
    printf("Server root dir set to: %s\n", root_dir);
    int clients[CONNMAX] = {0}; /*   1000 client cap, to avoid small-scale DDOS    */
    int listen_sock, new_sock;  /* client init to 0, indicates no client connected */
    char ch;                    /* getopt char holder */
    socklen_t addrlen;          /* client sock addr len */
    struct sockaddr_in client_addr;
    uint16_t port = htons(9630); //default port

    /*          parse the cmd line args          */
    //TODO: add error checking
    while((ch = getopt (argc, argv, "p:r:")) != -1) {
        switch (ch) {
            case 'r':
                root_dir = malloc(strlen(optarg));
                strcpy(root_dir, optarg);
                break;
            case 'p':
                port = htons(optarg);
                break;
            case '?':
                fprintf(stderr, "Wrong arguments given!!!\n");
                exit(1);
            default:
                exit(1);
        }
    }
    listen_sock = start_server(port);

    int slot = 0;
    while (1) {
        addrlen = sizeof(client_addr);

        if ((clients[slot] = accept(listen_sock, (struct sockaddr*) &client_addr, &addrlen)) < 0) {
            perror("Error accepting traffic on socket slot:");
            exit(1);
        }
        else if (clients[slot] > 0) {
            printf("Client connection established on slot: %i\n", slot);
            if ( fork() == 0 ) {
                respond(slot);
                exit(0);
            }
        }

        while (clients[slot]!=-1) {
            slot = (slot + 1) % CONNMAX;
        }
    }
    return 0;
}

int start_server(char *port) {
    int listen_sock;
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* get host address info */
    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        perror("Error occurred while getting server address info:");
        exit(1);
    }

    /* create fd's for listening sock poss addresses */
    for (p = res; p != NULL; p = p->ai_next) {
        if ((listen_sock = socket(p->ai_family, p->ai_socktype, 0)) > 0) {
            /*                bind the sock            */
            if (bind(listen_sock, p->ai_addr, p->ai_addrlen) == 0) {
                printf("Server socket bound to port: %s\n", port); break;
            }
        }
    }
    if (p == NULL || listen_sock == NULL) {
        perror("Error occurred while creating or binding listen_sock");
        exit(1);
    }
    freeaddrinfo(res);

    /* listen for incoming connections */
    if (listen (listen_sock, 1000000) != 0) {
        perror("Error listening listening for clients:");
        exit(1);
    }
    return listen_sock;
}

void respond(int n) { /* serve response for client request */
    char mesg[99999], *reqline[3], data_to_send[SOCKBUFF], path[99999];
    int rcvd, fd, bytes_read;

    memset( (void*)mesg, (int)'\0', 99999 );

    rcvd=recv(clients[n], mesg, 99999, 0);

    if (rcvd<0) {   /* receive error */
        fprintf(stderr, ("recv() error\n"));
    }
    else if (rcvd==0) {   /* receive socket closed */
        fprintf(stderr, "Client disconnected upexpectedly.\n");
    }
    else { /* message received */
        printf("%s", mesg);
        reqline[0] = strtok (mesg, " \t\n");
        if (strncmp(reqline[0], "GET\0", 4)==0) {
            reqline[1] = strtok (NULL, " \t");
            reqline[2] = strtok (NULL, " \t\n");

            if (strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp(reqline[2], "HTTP/1.1", 8)!=0) {
                write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
            }

            else {
                /* If no file specified, index.html will be opened by default */
                if (strncmp(reqline[1], "/\0", 2) == 0) {
                    reqline[1] = "/index.html";
                }

                strcpy(path, root_dir);
                strcpy(&path[strlen(root_dir)], reqline[1]);
                printf("file: %s\n", path);

                if ( (fd=open(path, O_RDONLY))!=-1 ) { /* FILE FOUND */
                    send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
                    while ((bytes_read=read(fd, data_to_send, SOCKBUFF)) > 0) {
                        write(clients[n], data_to_send, bytes_read);
                    }
                }
                else { /* FILE NOT FOUND */
                    write(clients[n], "HTTP/1.0 404 Not Found\n", 23);
                }
            }
        } /* CLOSING client socket */
    }  /* Send and receive operations are DISABLED */
    shutdown(clients[n], SHUT_RDWR);
    close(clients[n]);
    clients[n]=-1;
}


///* write buffer to new sock */
//recv(new_sock, buffer, bufsize, 0);
//printf("%s\n", buffer);
///* write RESPONSE headers to sock */
//write(new_sock, "HTTP/2.0 200 OK\n", 16);
//write(new_sock, "Content-length: 46\n", 19);
//write(new_sock, "Content-Type: text/html\n", 26);
//write(new_sock, "Accept-Ranges: bytes\n", 21);
//write(new_sock, "Connection: close\n", 18);
//write(new_sock, "Content-Type: text/html; charset=UTF-8\n", 39);
//write(new_sock, "Content-Encoding: gzip,deflate\n", 31);
//write(new_sock, "<html><body><H1>Welcome to the PwdManLib dashboard!</H1></body></html>", 70);
//close(new_sock);


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