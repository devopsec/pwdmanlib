#include <stdio.h>
#include "util/stringops.h"
#include "server/fawkes_proto.h"

void printByteDebug(unsigned char byte);

int main() {
    unsigned char info;
    printf("Please enter a number between 0 and 255 \n");
    scanf("%hhu", &info);

    static const unsigned int bit1 = 1;    // 2^0    00000001
    static const unsigned int bit2 = 2;    // 2^1    00000010
    static const unsigned int bit3 = 4;    // 2^2    00000100
    static const unsigned int bit4 = 8;    // 2^3    00001000
    static const unsigned int bit5 = 16;   // 2^4    00010000
    static const unsigned int bit6 = 32;   // 2^5    00100000
    static const unsigned int bit7 = 64;   // 2^6    01000000
    static const unsigned int bit8 = 128;  // 2^7    10000000

    printf("Displaying original byte encoded info\n");
    printByteDebug(info);

    decodeReqmts(info);

    decodeProto(info);

    decodeMethod(info);

    /* setting info in the byte */

    printf("setting A BAD req's info\n");
    info = (info & 252) |  (0 & ~252);
    printByteDebug(info);/* ^pass in num to set here */
    decodeReqmts(info);

    printf("setting the protocol to HTTP/1.0\n");
    info = (info & 243) | ((1 << 2) & ~243);
    printByteDebug(info);/* ^pass in num to set here */

    printf("setting the method to GET\n");
    info = (info & 15)  | ((1 << 4) & ~15);
    printByteDebug(info);/* ^pass in num to set here */

    exit(0);
}
