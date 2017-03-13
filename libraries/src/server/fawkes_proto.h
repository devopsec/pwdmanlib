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

#ifndef PWDMANLIB_FAWKES_PROTO_H
#define PWDMANLIB_FAWKES_PROTO_H

/********************************************************************************
 *                  GuyFawkes Protocol Definitions                              *
 ********************************************************************************
 *                  - Abstraction of (RFC 822) -                                *
 *                  - Copyright 2017 DevOpSec  -                                *
 *                  -   All Rights Reserved    -                                *
 ********************************************************************************/

#include <stdio.h>
#include "stringops.h"
#include "constants.h"

short decodeProto(unsigned char info);
short decodeMethod(unsigned char info);
short decodeReqmts(unsigned char info);
unsigned char encodeProto(char *proto, unsigned char info);
unsigned char encodeMethod(char *method, unsigned char info);
unsigned char encodeReqmts(unsigned char info);
void printByteDebug(unsigned char byte);

/* TODO: these can all be simplified but are explicit to make functions clear */

/** sets protocol info, returns set info byte **/
unsigned char encodeProto(char *proto, unsigned char info) {
    unsigned char x;

    if (strncmp(proto, "HTTP/1.0\0", 9) == 0) {
        printf("Protocol detected: HTTP/1.0\n");
        x = 1;
    } else if (strncmp(proto, "HTTP/1.1\0", 9) == 0) {
        printf("Protocol detected: HTTP/1.1\n");
        x = 2;
    } else if (strncmp(proto, "HTTP/2.0\0", 9) == 0) {
        printf("Protocol detected: HTTP/2.0\n");
        x = 3;
    } else {
        printf(ANSI_COLOR_RED "Protocol not supported\n" ANSI_COLOR_RESET);
        printByteDebug(info);
        x = 0;
    }

    printf("setting the protocol\n");
    info = (info & 243) | ((x << 2) & ~243);
    return info;
}

/** sets method info, returns set info byte **/
unsigned char encodeMethod(char *method, unsigned char info) {
    unsigned char x;

    if (strncmp(method, "GET\0", 4) == 0) {
        printf("Processing GET request\n");
        x = 1;
    } else if (strncmp(method, "HEAD\0", 5) == 0) {
        printf("Processing HEAD request\n");
        x = 2;
    } else if (strncmp(method, "POST\0", 5) == 0) {
        printf("Processing POST request\n");
        x = 3;
    } else if (strncmp(method, "PUT\0", 4) == 0) {
        printf("Processing PUT request\n");
        x = 4;
    } else if (strncmp(method, "DELETE\0", 7) == 0) {
        printf("Processing DELETE request\n");
        x = 5;
    } else if (strncmp(method, "CONNECT\0", 8) == 0) {
        printf("Processing CONNECT request\n");
        x = 6;
    } else if (strncmp(method, "OPTIONS\0", 8) == 0) {
        printf("Processing OPTIONS request\n");
        x = 7;
    } else if (strncmp(method, "TRACE\0", 6) == 0) {
        printf("Processing TRACE request\n");
        x = 8;
    } else if (method == "" || method == NULL) {
        printf(ANSI_COLOR_RED "Request method missing\n" ANSI_COLOR_RESET);
        printByteDebug(info);
        x = 0;
    } else {
        printf(ANSI_COLOR_RED "Request method not supported\n" ANSI_COLOR_RESET);
        printByteDebug(info);
        x = 0;
    }

    printf("setting the method\n");
    info = (info & 15)  | ((x << 4) & ~15);
    return info;
}

/**   checks method any proto and if reqs met   **
 ** sets the requirements, returns set info byte  **/
unsigned char encodeReqmts(unsigned char info) {
    unsigned char x;

    if (((info >> 2) & 3) != 0 && (info >> 4) != 0 && (info >> 4) < 9) {
        printf("Requirements inspection passed\n");
        x = 1;
    }
//    else if () {
//        printf("Malformed request\n");
//        printByteDebug(info);
//        return -1;
//    } else if () {
//        printf("Requirements not met\n");
//        printByteDebug(info);
//        return -1;
//    }
    else { /* check accompanying signature */
        printf(ANSI_COLOR_RED "Malformed information byte (DROP)\n" ANSI_COLOR_RESET);
        printByteDebug(info);
        x = 0;
    }

    printf("setting reqmts info\n");
    info = (info & 252) |  (x & ~252);
    return info;
}

/** gets protocol info, returns -1 on error **/
short decodeProto(unsigned char info) {
    unsigned char x;

    if (((info >> 2) & 3) == 1) {
        printf("Protocol detected: HTTP/1.0\n");
        return 1;
    } else if (((info >> 2) & 3) == 2) {
        printf("Protocol detected: HTTP/1.1\n");
        return 2;
    } else if (((info >> 2) & 3) == 3) {
        printf("Protocol detected: HTTP/2.0\n");
        return 3;
    } else {
        printf("Protocol not supported\n");
        printByteDebug(info);
        return -1;
    }
}

/** gets method info, returns -1 on error **/
short decodeMethod(unsigned char info) {
    unsigned char x;

    if ((info >> 4) == 1) { /* 16-31 */
        printf("Processing GET request\n");
        return 1;
    } else if ((info >> 4) == 2) { /* 32-47 */
        printf("Processing HEAD request\n");
        return 2;
    } else if ((info >> 4) == 3) { /* 48-63 */
        printf("Processing POST request\n");
        return 3;
    } else if ((info >> 4) == 4) { /* 64-79 */
        printf("Processing PUT request\n");
        return 4;
    } else if ((info >> 4) == 5) { /* 80-95 */
        printf("Processing DELETE request\n");
        return 5;
    } else if ((info >> 4) == 6) { /* 96-111 */
        printf("Processing CONNECT request\n");
        return 6;
    } else if ((info >> 4) == 7) { /* 112-127 */
        printf("Processing OPTIONS request\n");
        return 7;
    } else if ((info >> 4) == 8) { /* 128-143 */
        printf("Processing TRACE request\n");
        return 8;
    } else if ((info >> 4) == 0) { /* 0-15 */
        printf("Request method missing\n");
        printByteDebug(info);
        return -1;
    } else {
        printf("Request method not supported\n");
        printByteDebug(info);
        return -1;
    }
}

/** gets the requirements, returns -1 on error **/
short decodeReqmts(unsigned char info) {
    unsigned char x;

    if ((info & 3) == 1) {
        printf("Requirements inspection passed\n");
        return 1;
    } else if ((info & 3) == 2) {
        printf(ANSI_COLOR_RED "Malformed request\n" ANSI_COLOR_RESET);
        printByteDebug(info);
        return -1;
    } else if ((info & 3) == 3) {
        printf("Requirements not met\n");
        printByteDebug(info);
        return -1;
    } else {
        printf("Malformed information byte ($DROP)\n");
        printByteDebug(info);
        return -1;
    }

}

void printByteDebug(unsigned char byte) {
    int i;
    printf(ANSI_COLOR_YELLOW "Byte decimal: %i\n", byte);
    printf("Byte binary : ");
    for (i = 0; i < 8; i++) {
        printf("%d", ((byte << i) & 0x80) != 0);
    } printf("\n" ANSI_COLOR_RESET);
}

#endif //PWDMANLIB_FAWKES_PROTO_H
