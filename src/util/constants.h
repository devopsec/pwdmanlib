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
 *                 Define Platform-specific constants here                      *
 ********************************************************************************/
#ifndef PWDMANLIB_CONSTANTS_H
#define PWDMANLIB_CONSTANTS_H

#include <limits.h>
#include <stddef.h>

#define         MAX_BUF_SIZE            4096 //files
#define         LINE_MAX                2048
#define         PATH_MAX                4096
#define         CONNMAX                 1000000
#define         HOSTNAME_MAX            255

/* dealing with that pesky ssize_t                 *
 * may need to be hardcoded for 20-bit controllers */
//#if SIZE_MAX == UINT_MAX
//    typedef int ssize_t;        /* common 32 bit case */
//#elif SIZE_MAX == ULONG_MAX
//    typedef long ssize_t;       /* linux 64 bits */
//#elif SIZE_MAX == ULLONG_MAX
//    typedef long long ssize_t;  /* windows 64 bits */
//#elif SIZE_MAX == USHRT_MAX
//    typedef short ssize_t;      /* is this even possible? */
//#else /* try some of the exotic cases */
//    #if defined(_MSC_VER)       /* newer windows vers */
//        #include <BaseTsd.h>    /* containes typedef */
//    #endif
//    #ifndef ssize_t
//        #error platform has exotic SIZE_MAX
//#endif

/* These should be set on server startup */
#define         SOCK_BUFF_MIN_READ      4096
#define         SOCK_BUFF_DEF_READ      87380
#define         SOCK_BUFF_MAX_READ      6291456
#define         SOCK_BUFF_MIN_WRITE     4096
#define         SOCK_BUFF_DEF_WRITE     16384
#define         SOCK_BUFF_MAX_WRITE     4194304
#define         MAX_QUEUE               10000

/* colors for terminal printing */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#endif //PWDMANLIB_CONSTANTS_H

/* misc code will be added in later */
//#if __INTELLISENSE__
//#endif // windows
