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

#ifndef PWDMANLIB_STRINGOPS_H
#define PWDMANLIB_STRINGOPS_H

/********************************************************************************
 *                    Utility classes for working with strings                  *
 ********************************************************************************/

#define XOR_SWAP(a,b) do {a ^= b; b ^= a; a ^= b;} while (0)
#if defined(_WIN32) || defined(__CYGWIN__)
# define strtok_r strtok_s
#endif

#include <stdlib.h>
#include <string.h>
#include <memory.h>

/* pre-declarations */
char *readFile(char *path);
void strReverse(char *str);
char *itoa(int value, int radix);
char **strsplit(char* str, const char delims[], size_t *len);
void joinPath(char *destination, const char *path1, const char *path2);

/* TODO: add substring parsing capabilities */
/**********************************************************************************
 \t     summary      -    function for tokenizing strings (extends strtok function)
 \t     params       -    char* <string>, const char[] <delims>, size_t* <empty ptr>
 \t     returns      -    a ptr to an array of ptrs to strings
 \t     sets         -    len to length of string arr (number of strings)
 \t     notes        -    function is thread-safe, delims are destroyed
 \t     notes        -    len is set to 0 on error (you should check for this)
 \t     notes        -    consecutive delims will set token as empty string "" in array
 \t     notes        -    the return array is terminated with a null ptr '\0'
 **********************************************************************************/
char **strsplit(char *str, const char delims[], size_t *len) {
    char *save, *tok;        /* holds tok val btwn calls */
    char **result = '\0';    /* set result to NULL */
    char *tmp = strdup(str); /* leaves original str intact */
    size_t delims_size = strlen(delims);
    size_t count = 0;        /* number of main strings */
    size_t sub_count = 0;    /* number of substrings */
    int i = 0;

    /* get number of delims in str */
    while ((tmp = strstr(tmp, delims)) != NULL) {
        tmp += delims_size;
        count++;
    } count++; /* add one for trailing token */
    tmp = strdup(str);

    save = malloc(sizeof(char) * strlen(str));
    result = malloc(sizeof(char *) * count);

    if (result && save) {
        while ((tok = strstr(tmp, delims)) != NULL) {
            strncpy(save, tmp, (tok - tmp));
            save[(tok - tmp)] = '\0';
//            printf("Token extracted: <<%s>>\n", save);
            result[i++] = strdup(save);
            tok += delims_size;
            tmp = tok;
        } /* grab trailing token */
        if (tmp && tmp[0] != '\0') {
            result[i++] = strdup(tmp);
//            printf("Token extracted: <<%s>>\n", tmp);
        } /* set last ptr to NULL */
        result[i] = '\0';
        *len = count; /* pass num toks */
    }
    else { /* set len to 0 on error */
        *len = 0;
    }
    if (save) free(save);
    return result;
}

/********************************************
 \t read file into a buffer (one large str)
 \t if error occurs returns "-1\0" as str
 ********************************************/
char *readFile(char *path) {
    char *source = NULL;
    FILE *fp = fopen(path, "rb");
    if (fp != NULL) {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            long bufsize = ftell(fp);
            if (bufsize == -1) {
                fputs("Error reading file", stderr);
                source = itoa(-1, 10);
                return source;
            }

            /* Allocate our buffer to that size. */
            source = malloc(sizeof(char) * (bufsize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) {
                fputs("Error reading file", stderr);
                source = itoa(-1, 10);
                return source;
            }

            /* Read the entire file into memory. */
            size_t fileLen = fread(source, sizeof(char), bufsize, fp);
            if (ferror(fp) != 0) {
                fputs("Error reading file", stderr);
                source = itoa(-1, 10);
                return source;
            }
            else {
                source[fileLen++] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
    }
    return source;
}

/** convert int to str - this is portable **/
char *itoa(int value, int radix) {
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* str = malloc(sizeof(int));
    int n = 0, neg = 0;
    unsigned int v;
    char* p, *q;
    char c;

    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }

    v = value;
    do {
        str[n++] = dig[v%radix];
        v /= radix;
    } while (v);

    if (neg) str[n++] = '-';
    str[n] = '\0';

    for (p = str, q = p + (n-1); p < q; ++p, --q) {
        c = *p, *p = *q, *q = c;
    }
    return str;
}

/** reverse a string in place **/
void strReverse(char *str) {
    int start = 0;
    size_t end = strlen(str) -1;
    while (start < end) {
        XOR_SWAP(*(str+start), *(str+end));
        start++;
        end--;
    }
}

/* append a path to another path and set destination result */
void joinPath(char *destination, const char *path1, const char *path2) {
    if(path1 == NULL && path2 == NULL) {
        strcpy(destination, "");;
    }
    else if (path2 == NULL || strlen(path2) == 0) {
        strcpy(destination, path1);
    }
    else if (path1 == NULL || strlen(path1) == 0) {
        strcpy(destination, path2);
    }
    else {
        char directory_separator[] = "/";
#ifdef _WIN32
        directory_separator[0] = '\\';
#endif
        const char *last_char = path1;
        while(*last_char != '\0')
            last_char++;
        int append_directory_separator = 0;
        if(strcmp(last_char, directory_separator) != 0) {
            append_directory_separator = 1;
        }
        strcpy(destination, path1);
        if(append_directory_separator)
            strcat(destination, directory_separator);
        strcat(destination, path2);
    }
}

#endif //PWDMANLIB_STRINGOPS_H
