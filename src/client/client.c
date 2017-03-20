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
 *                    Client-side networking functions                          *
 *                   cmd line opts: -p [port] -r [path]                         *
 *                long opts: --port [port] --root-dir [path]                    *
 ********************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "client.h"
#include "util/constants.h"
#include "crypto/ssl_funcs.h"

#define USAGE "Usage: cmd line options: -p [port] -r [path]\n"\
              "Or using long options: --port [port] --root-dir [path]\n"\
              "Both cmd line args are optional and have default values below:\n"\
              "path == Path to dir containing resources to send in post req's.\n"\
              "Path defaults to dir where client was started from.\n"\
              "port == Port for connection to server (need su/root for < 1024)\n"\
              "Port defaults to 65501.\nTo exit program press Ctrl+C.\n"

/* function declarations */
int receive();
void sig_handler(int signo);
void parent_sig_handler(int signo);
void child_sig_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int sendall(SSL *ssl, char *strbuf, int *len);
int readall(SSL *ssl, char *buf);

/* DEBUG: TEMP global vars
 * TODO: either encapsulate or create funcs for passing */
int worker_sock;                         /* new connection on worker_sock */
int connect_sock;                        /* server listening sock that workers connect to */
char *root_dir;                          /* root dir for files / data to send to server */

/* TODO: section into library functions */
int main(int argc, char* argv[]) {
    socklen_t addrlen;                          /* client sock addr len */
    struct sockaddr_storage client_addr;        /* where the client is connecting from */
    char *port = "65501";                       /* if set below 5 chars must malloc in arg parsing */
    char s[INET6_ADDRSTRLEN];                   /* address needs to be big enough to hold ipv4 or ipv6*/

    if (signal(SIGINT, parent_sig_handler) == SIG_ERR || signal(SIGHUP, parent_sig_handler) == SIG_ERR ||
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
    while ((token = strsep(argv, delim)) != NULL) {
        if (option_flag != 0) {
            if (option_flag == 1) {
                memset((void *) port, (int) '\0', sizeof(port));
                port = token;
                option_flag = 0;
            }
            if (option_flag == 2) {
                root_dir = malloc(strlen(token));
                strcpy(root_dir, token);
                option_flag = 0;
            }
        } else {
            if (token == "-p" || token == "--port") {
                option_flag = 1;
            } else if (token == "-r" || token == "--root-dir") {
                option_flag = 2;
            }
        }
    }

    if (root_dir == NULL) {
        char *dir = getenv("PWD");
        malloc(strlen(dir));
        root_dir = malloc(strlen(dir) + 1);
        free(dir);
    }

    printf("Client root dir set to: %s\n", root_dir);
    printf("Client connection port set to: %s\n", port);
    start_client(port);
}

void start_client(char *port) {
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(argv[1], port, &hints, &servinfo) != 0) {
        perror("Error occurred while getting server address info\n");
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((connect_sock = socket(p->ai_family, p->ai_socktype,
                                   p->ai_protocol)) == -1) {
            perror("client: socket"); continue;
        }

        if (connect(connect_sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(connect_sock);
            perror("client: connect"); continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure
}

//int receive() {
//    char *reqline[3], path[PATH_MAX] = {'\0'};
//    unsigned char data[SOCK_BUFF_MAX_READ] = {'\0'};
//    unsigned char recv_data[SOCK_BUFF_DEF_READ] = {'\0'};
//    unsigned char resp_data[SOCK_BUFF_DEF_WRITE] = {'\0'};
//    ssize_t bytes_read, total_bytes = 0;
//
//
//    do { /* read into buff in loop to ensure we get full response */
//        if ((bytes_read = recv(worker_sock, recv_data, SOCK_BUFF_DEF_READ, 0)) < 0) {
//            fprintf(stderr, ("recv() error\n")); break;
//        } // <-- break out, try to process data
//        memcpy(data, recv_data, sizeof(recv_data) + 1);          // copy into mesg buff
//        memset((void*)recv_data, (int)'\0', SOCK_BUFF_DEF_READ); // null out read buff
//        total_bytes += bytes_read;                               // counter
//    } while (bytes_read != 0 && total_bytes < SOCK_BUFF_MAX_READ);
//
//    printf("client: received '%s'\n", recv_data);
//    close(connect_sock);
//    return 0;
//}

/* get sockaddr, IPv4 or IPv6: */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sig_handler(int signo) {
    /* Hang up detected or death of process */
    if (signo == SIGHUP) {
        printf("\nReceived SIGHUP, exiting..\n");
        close(worker_sock);
        exit(1);
    }
    else if (signo == SIGINT) {
        printf("\nReceived SIGINT exiting..\n");
        close(worker_sock);
        exit(2);
    }
    else if (signo == SIGQUIT) {
        printf("\nReceived SIGQUIT, exiting..\n");
        close(worker_sock);
        exit(3);
    }
    else if (signo == SIGSEGV) {
        printf("\nMemory allocation error occurred, exiting..\n");
        close(worker_sock);
        exit(-1);
    }
}

//// first, load up address structs with getaddrinfo():
//
//memset(&hints, 0, sizeof hints);
//hints.ai_family = AF_UNSPEC;
//hints.ai_socktype = SOCK_STREAM;
//
//getaddrinfo("www.example.com", "3490", &hints, &res);
//
//// make a socket:
//
//sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//
//// connect!
//
//connect(sockfd, res->ai_addr, res->ai_addrlen);
