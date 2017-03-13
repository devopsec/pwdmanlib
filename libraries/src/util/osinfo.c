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

#ifdef _WIN64
//define something for Windows (64-bit)
#elif _WIN32
//define something for Windows (32-bit)
#elif __APPLE__
#include "TargetConditionals.h"
    #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
        // define something for simulator
    #elif TARGET_OS_IPHONE
        // define something for iphone
    #else
        #define TARGET_OS_OSX 1
        // define something for OSX
    #endif
#elif __ANDROID__
//some android specific stuff
#elif __linux
// linux
#elif __unix // all unices not caught above
// Unix
#elif __posix
    // POSIX
#endif

char *get_absolute_path(char *relativepath) {
    char *abs_path = (char*) malloc(PATH_MAX + 1);
    char *which = "which ";
    char *whichcmd = (char *) malloc(1 + strlen(which) + strlen(relativepath));

    /* try realpath */
    if (realpath(relativepath, abs_path) != NULL) {
        printf("realpath set abs_path to: %s\n", abs_path);
        return abs_path;
    }

    /* try opening and reading */
    strcpy(whichcmd, which);
    strcat(whichcmd, relativepath);
    FILE *fp = popen(whichcmd, "r");
    if (fgets(abs_path, PATH_MAX+1, fp) != NULL) {
        abs_path[strlen(abs_path)-1] = '\0';
        pclose(fp);
        free(whichcmd);
        return abs_path;
    }   pclose(fp);     /* make sure its when conditional not execcuted */
    free(whichcmd);

    /* try readlink */
    ssize_t status = readlink(relativepath, abs_path, sizeof(PATH_MAX));
    if (status != -1) {
        abs_path[strlen(abs_path)] = '\0';
        return abs_path;
    }

    printf("No absolute path found for cmd\nTrying relative path..\n");
    set_environ_flag = 1;
    abs_path = relativepath;
    return abs_path;
}