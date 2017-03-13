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
 *              long opts: --port [port] --root-dir [path]                      *
 ********************************************************************************/

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "server.h"
#include "util/constants.h"

#define USAGE "Usage: cmd line options: -p [port] -r [path]\n"\
              "Or using long options: --port [port] --root-dir [path]\n"\
              "Both cmd line args are optional and have default values below:\n"\
              "path == Path to dir containing resources to serve to client.\n"\
              "Path defaults to dir where server was started from.\n"\
              "port == Port to listen for client connections (need su/root for < 1024)\n"\
              "Port defaults to 65501.\nTo exit program press Ctrl+C.\n"

/* function declarations */
void start_server(char *);
void respond(int worker_sock, char *root_dir);
void set_sig_handler(void *sig_handler);
void parent_sig_handler(int signo);
void child_sig_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int sendall(int s, char *buf, ssize_t *len);

/* DEBUG: TEMP global vars
 * TODO: encapsulate / create funcs for passing */
int listen_sock;                         /* server listening sock that workers connect to */


/* TODO: section into library functions */
int main(int argc, char *argv[]) {
    char *root_dir = '\0';               /* root dir for html files, js, etc.. */
    int worker_sock;                     /* new connection on worker_sock */
    socklen_t addrlen;                   /* client sock addr len */
    struct sockaddr_storage client_addr; /* where the client is connecting from */
    char *port = "65501";                /* if set below 5 chars must malloc in arg parsing */
    char s[INET6_ADDRSTRLEN];            /* address needs to be big enough to hold ipv4 or ipv6*/


    //set_sig_handler(parent_sig_handler);
    /* DEBUG: this is temp, replace with more robust functions */
    if (signal(SIGINT, parent_sig_handler)  == SIG_ERR || signal(SIGHUP, parent_sig_handler)  == SIG_ERR ||
        signal(SIGSEGV, parent_sig_handler) == SIG_ERR || signal(SIGQUIT, parent_sig_handler) == SIG_ERR) {
        perror("Can't set signal handler");
        exit(-1);
    }

    /*          parse the cmd line args          */
    if (argc > 4 || argc < 0) { /* both args are optional */
        printf("Too many command line args provided.\n%s", USAGE);
        exit(1);
    }

    unsigned short option_flag = 0;
    char *token, *delim = " ";
    /* TODO: change to strtok_r == POSIX, strtok_s == WIN for portablility */
    while ((token = strsep(argv, delim)) != NULL) {
        if (option_flag != 0) {
            if (option_flag == 1) {
                memset((void*)port, (int)'\0', sizeof(port));
                port = token;
                option_flag = 0;
            }
            if (option_flag == 2) {
                root_dir = malloc(strlen(token) + 1);
                strcpy(root_dir, token);
                option_flag = 0;
            }
        }
        else {
            if (token == "-p" || token == "--port") {
                option_flag = 1;
            }
            else if (token == "-r" || token == "--root-dir") {
                option_flag = 2;
            }
        }
    }

    /* TODO: need to fix line ending on this (getcwd) */
    /* set root_dir if not set */
    if (root_dir == NULL) {
        root_dir = getenv("PWD");
    }

    printf("Server root dir set to: %s\n", root_dir);
    printf("Server listening port set to: %s\n", port);
    start_server(port); /* listen_sock will be set here */

    while (1) { /* accepting client connections */
        addrlen = sizeof(client_addr);

        /* create worker and accept traffic on listen_sock */
        printf("Waiting for client connection\n");
        if ((worker_sock = accept(listen_sock, (struct sockaddr*) &client_addr, &addrlen)) < 0) {
            perror("Error accepting traffic on socket:"); continue;
        }

        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), s, sizeof(s));
        printf("server: got connection from %s\n", s);

//        if (!fork()) {                       // this is the child process
//            close(listen_sock);               // child doesn't need the listener
        respond(worker_sock, root_dir);             // send response to client
//            shutdown(worker_sock, SHUT_RDWR); // block connections
        close(worker_sock);               // destroy that old sock!
        exit(0);                          // exit proc
//        }                                     // end of child proc
//        close(worker_sock);                   // parent doesn't need this
    }
    return 0;
}

void start_server(char *port) {
    int yes = 1;
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // use ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my ip

    /* get host address info */
    if (getaddrinfo(NULL, port, &hints, &servinfo) != 0) {
        perror("Error occurred while getting server address info\n");
        exit(1);
    }

    /* loop through all the results and bind to the first we can */
    for (p = servinfo; p != NULL; p = p->ai_next) { /*  bind the sock  */
        /* create fd for listening on sock poss addresses */
        if ((listen_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket"); continue;
        }

        /* lose the pesky "Address already in use" error message */
        if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            perror("Error on socket: setsockopt\n");
            exit(1);
        }

        /* make sure to close sockets that failed to bind */
        if (bind(listen_sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(listen_sock);
            perror("server: socket bind");
            continue;
        }
        else { /* if its good bind it! */
            printf("Server socket bound to port: %s\n", port); break;
        }
    } freeaddrinfo(servinfo);

    /*  error checking  */
    if (p == NULL || listen_sock == NULL) {
        perror("Error occurred while creating or binding listen_sock\n");
        exit(1);
    }

    /* listen for incoming connections
     * allow for up to 10000 clients in queue */
    if (listen(listen_sock, MAX_QUEUE) != 0) {
        perror("Error listening listening for clients\n");
        exit(1);
    }

    /* reap all dead processes */
    sa.sa_handler = child_sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

/* serve response for client request */
void respond(int worker_sock, char *root_dir) {
    char recv_data[SOCK_BUFF_DEF_READ] = {'\0'};
    ssize_t bytes_read = 0;
    int resourcefd;

    /* TODO: change this to terminate on client req flag (header w/ size of req) */
    /* read into buff full request */
    if ((bytes_read = recv(worker_sock, recv_data, SOCK_BUFF_DEF_READ, 0)) < 0) {
        fprintf(stderr, ("recv() error\n"));
    } //DEBUG
    //printf("File: %s\nFunction: %s()\nLine Number: %d\nbytes_read: %d, MAX: %d\n",__FILE__,__FUNCTION__,__LINE__, bytes_read, SOCK_BUFF_MAX_READ);
    /* error checking buffer overflow checking */
    if (bytes_read == 0 || bytes_read > SOCK_BUFF_MAX_READ) {
        fprintf(stderr, ("recv() error\n"));
        /* send error response */
    }

    Request request = parseRequest(recv_data);                  /* parse the request and perform error checking */
    routeClient(request, worker_sock, root_dir);                /* route client based on parsed request         */
}

/* TODO: pull all of these utility funcs and defs into header */
/* TODO: make child handler conform to new abstraction */
void set_sig_handler(void *sig_handler) {
    if (signal(SIGINT, sig_handler)  == SIG_ERR || signal(SIGHUP, sig_handler)  == SIG_ERR ||
        signal(SIGSEGV, sig_handler) == SIG_ERR || signal(SIGQUIT, sig_handler) == SIG_ERR) {
        perror("Can't set signal handler");
        exit(-1);
    }
}
/* TODO: add graceful shutdown and error logging */
void parent_sig_handler(int signo) {
    /* Hang up detected or death of process */
    if (signo == SIGHUP) {
        printf("\nReceived SIGHUP, exiting..\n");
        close(listen_sock);
        exit(1);
    }
    else if (signo == SIGINT) {
        printf("\nReceived SIGINT exiting..\n");
        close(listen_sock);
        exit(2);
    }
    else if (signo == SIGQUIT) {
        printf("\nReceived SIGQUIT, exiting..\n");
        close(listen_sock);
        exit(3);
    }
    else if (signo == SIGSEGV) {
        printf("\nMemory allocation error occurred, exiting..\n");
        close(listen_sock);
        exit(-1);
    }
}

/* waitpid() might overwrite errno, so we save and restore it */
void child_sig_handler(int s) {
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

/* get sockaddr, IPv4 or IPv6: */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
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

/*
//  Now you can extend this with other functions such as:
//  Serving files (you can use sendfile() function)
//  Serving different content types(images, zip, pdf etc)
//  Different processes to handle requests (fork())
//  Other http status codes
//  Processing PHP (process PHP with system commands and write the result to the socket)
 */