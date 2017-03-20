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

#ifndef PWDMANLIB_NETOPS_H
#define PWDMANLIB_NETOPS_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int get_ip(char *hostname, char *ip);

int main(int argc , char *argv[]) {
    if(argc <2) {
        printf("Please provide a hostname");
        exit(1);
    }
    char *hostname = argv[1];
    char ip[100];
    get_ip(hostname , ip);
    printf("%s resolved to %s" , hostname , ip);
    printf("\n");
}

int get_ip(char *hostname, char *ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname() error: ");
        return -1;
    }

    addr_list = (struct in_addr**) he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }
    return -1;
}
int ip2Domain(char *ip) {
    int status;
    struct hostent *he;
    struct in_addr ipv4addr;
    struct in6_addr ipv6addr;

    status = inet_pton(AF_INET, ip, &ipv4addr);
    if (status == 1) {
        he = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);
        if (he) {
            fprintf(stdout, "Host name: %s\n", he->h_name);
            return 0;
        }
    }
    else if (status == -1) {
        perror("inet_pton() error: ");
        return -1;
    }

    status = inet_pton(AF_INET6, ip, &ipv6addr);
    if (status == 1) {
        he = gethostbyaddr(&ipv6addr, sizeof(ipv6addr), AF_INET6);
        if (he) {
            fprintf(stdout, "Host name: %s\n", he->h_name);
            return 0;
        }
    }
    else if (status == -1) {
        perror("inet_pton() error: ");
        return -1;
    }
    return -1;
}

int domain2IpRange(const char *domain) {
    struct addrinfo *result;
    struct addrinfo *res;
    int status;

    /* resolve the domain name into a list of addresses */
    char *hostname = malloc(sizeof(char) * NI_MAXHOST);
    status = getaddrinfo(domain, NULL, NULL, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
        if (hostname) free(hostname);
        return -1;
    }

    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {
        hostname = "";
        status = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
        if (status != 0) {
            fprintf(stderr, "getnameinfo() error: %s\n", gai_strerror(status));
            continue;
        }
        if (hostname) {
            printf("hostname: %s\n", hostname);
        }
    }

    if (hostname) free(hostname);
    if (result) freeaddrinfo(result);
    return 0;
}


#endif //PWDMANLIB_NETOPS_H
