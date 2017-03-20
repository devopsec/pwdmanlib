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

#ifndef PWDMANLIB_CLIENT_H
#define PWDMANLIB_CLIENT_H

/********************************************************************************
 *                    Client-side networking functions                          *
 ********************************************************************************/

/** sets server, sets serv_addr, creates sock, sets address on sock   **
 **  and attempts to connect, returning connected sock or -1 on error **/
int openConnection(const char *host, int port, struct hostent *server) {
    int sockfd;
    struct sockaddr_in serv_addr;

    /* get host info */
    server = gethostbyname(host);
    if (server == NULL) {
        perror("ERROR host doesn't exist\n");
        return -1;
    }

    /* create sock */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket\n");
        return -1;
    }

    /* fill in the structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting\n");
        return -1;
    }

    return sockfd;
}

#endif //PWDMANLIB_CLIENT_H
