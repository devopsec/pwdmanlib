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

#define USAGE "Usage: cmd line options: -p [port] -r [path]\n"\
              "Or using long options: --port [port] --root-dir [path]\n"\
              "Both cmd line args are optional and have default values below:\n"\
              "path == Path to dir containing resources to serve to client.\n"\
              "Path defaults to dir where server was started from.\n"\
              "port == Port to listen for client connections (need su/root for < 1024)\n"\
              "Port defaults to 65501.\nTo exit program press Ctrl+C.\n"\
              "Note: must be run with elevated priveledges (as root user).\n"

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

    short ret = isRoot();
    if (ret == -1) {
        printf("Server must be executed as root/sudo user\n");
        exit(1);
    }

    /*          parse the cmd line args          */
    if (argc > 4 || argc < 0) { /* both args are optional */
        printf("Too many command line args provided.\n%s", USAGE);
        exit(1);
    }

    unsigned short option_flag = 0;
    char *token, *delim = " ";
    /* TODO: change to self implemented parser for portablility */
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

    /* loop until signal received, perform clean up on signal and exit */
    while (sig_action_flag == 0) { /* accepting client connections */
        addrlen = sizeof(client_addr);

        /* create worker and acc ept traffic on listen_sock */
        printf("Waiting for client connection\n");
        if ((worker_sock = accept(listen_sock, (struct sockaddr*) &client_addr, &addrlen)) < 0) {
            perror("Error accepting traffic on socket:"); continue;
        }

        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), s, sizeof(s));
        printf("server: got connection from %s\n", s);

//        if (!fork()) {                          // this is the child process
//            close(listen_sock);                 // child doesn't need the listener
//            close (STDIN_FILENO);               // in prod child doesn't need
//            close (STDOUT_FILENO);              // in prod child doesn't need
            respond(worker_sock, root_dir);     // send response to client
//            close(worker_sock);                 // destroy that old sock!
//            exit(0);                            // exit proc
//        }                                       // end of child proc
        close(worker_sock);                     // parent doesn't need this
    }

    if (listen_sock) {
        close(listen_sock);
    }
    destroySSL(NULL, NULL);
    exit(sig_action_flag);
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