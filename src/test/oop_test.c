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
 *               Testing for Utility classes and OOP functions                  *
 ********************************************************************************/

#if defined(_WIN32) || defined(__CYGWIN__)
# define strtok_r strtok_s
#endif
#define MAX_STACK_FRAMES 64

#include "../util/oop.h"
#include "../util/stringops.h"

#include <signal.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>
#include <stdint.h>
#include <sys/time.h>

/* Pre-Declaration of Funcs */
double calcExecTime(struct timeval tv1, struct timeval tv2);
int addr2line(char const * const PROG_NAME, void const * const addr);
static void *stack_traces[MAX_STACK_FRAMES];
static uint8_t alternate_stack[SIGSTKSZ];
void sig_handler(int signo, siginfo_t *siginfo, void *context);
void set_sig_handler();

/* Pre-Declaration of Tests */
int test_dictionary();
int test_modules();
int test_virtual_class();

/* Global Vars */
char *read_file(char *path);
struct timeval  tv1, tv2;
unsigned short extended_tests_flag = 0;

int main(int argc, char* argv[]) {
    int status;
    char *tmp;

    /* parse cmd line args */
    if (argc > 1) {
        char *args = strdup(argv[1]);
        char *token = strtok_r(args, " ", &tmp);
        while (token != NULL) {
            if ((strcmp(token, "--extended")) == 0) {
                extended_tests_flag = 1;
            }
            token = strtok_r(NULL, " ", &tmp);
        }
        if (args) free(args);
    }
    set_sig_handler();

    if ((status = test_dictionary()) != 0) {
        printf("Dictionary Test FAILED\n");
        exit(status);
    } printf("Dictionary Test SUCCESS\n");
    exit(0);
}

/*******************           Virtual Class             ************************/

int test_virtual_class() {
    int status;
    tCommClass commTcp, commHttp;

    // Same 'base' class but initialised to different sub-classes.

    tcpInit(&commTcp);
    httpInit(&commHttp);

    // Called in exactly the same manner.

    status = (commTcp.open)(&commTcp, "bigiron.box.com:5000");
    if (status != 0) return -1;
    status = (commHttp.open)(&commHttp, "http://www.microsoft.com");
    if (status != 0) return -1;
    return 0;
}



/*******************              Modules                ************************/

int test_modules() {
   /* include type declaration after structure definition */

    Module *Module_create() { return (Module *) calloc(1, sizeof(Module)); }
    void Module_delete(Module *m) { free(m); }
    int Module_get_element(const Module *m) { return m->element; }
    void Module_set_element(Module *m, int e) { m->element = e; }

    return 0;
}

/******************         Dictionary (static-size)     ************************/

int test_dictionary() {
    int status = 0;
    char *val;
    size_t *namesArrLen = malloc(sizeof(size_t));
    size_t *stringsArrLen = malloc(sizeof(size_t));
    char **namesArr, **stringsArr;
    register char *namesBuf, *stringsBuf;

    Dict *dp = initDict(100);

    void clearmem() {
        if (namesBuf) {free(namesBuf);}
        if (stringsBuf) {free(stringsBuf);}
        if (namesArr) {free(namesArr);}
        if (stringsArr) {free(stringsArr);}
        if (namesArrLen) {free(namesArrLen);}
        if (stringsArrLen) {free(stringsArrLen);}
    }

    if (extended_tests_flag == 1) {
        unsigned short i = 50;
        double exec_time;
        long retconv;
        char *p;

        namesBuf = read_file("/CS202/TEMP/src/names.txt");
        namesArr = strsplit(namesBuf, "\n", namesArrLen);
        retconv = strtol(namesBuf, &p, 0);
        if (retconv == -1 || namesArrLen == 0) {
            status = 1;
        }
        printf("names arr len: %zi\n", *namesArrLen);

        stringsBuf = read_file("/CS202/TEMP/src/strings.txt");
        stringsArr = strsplit(stringsBuf, "\n", stringsArrLen);
        retconv = strtol(stringsBuf, &p, 0);
        if (retconv == -1 || stringsArrLen == 0) {
            status = 1;
        }
        printf("strings arr len: %zi\n", *stringsArrLen);

        printf("Filling 50 tuples with: Random name and 50-char string\n");
        gettimeofday(&tv1, NULL); //start fill

        do {
            --i;
            setTuple(dp, namesArr[i], stringsArr[i]);
        } while (i != 0);

        gettimeofday(&tv2, NULL); //end fill
        exec_time = calcExecTime(tv1, tv2);
        printf("Fill dict tuples exec time: %f seconds\n", exec_time);

        printf("Getting 50 tuple vals (50-char long) using: Random name from name list\n");
        stringsBuf[0] = '\0';
        gettimeofday(&tv1, NULL); //start fill
        /* TODO: segerror */
        i = 50;
        do {
            --i;
            val = (char*) getTuple(dp, namesArr[i]);
//            printf("%s\n", val);
        } while (i != 0);

        gettimeofday(&tv2, NULL); //end fill
        exec_time = calcExecTime(tv1, tv2);
        printf("Fill dict tuples exec time: %f seconds\n", exec_time);
    } if (status != 0) { clearmem(); return (-1); }

    status = setTuple(dp, "foo", "bar");
    if (status != 0) { clearmem(); return (-1); }
    printf("Value set to: bar\n");

    val = getIndex(dp, 0);
    if ((int) &val == -1) { clearmem(); return (-1); }
    printf("Value received: %s\n", (char *) val);

    status = setTuple(dp, "foo", "barbar");
    if ((int) &val == -1) { clearmem(); return (-1); }
    printf("Value set to: barbar\n");

    status = setTuple(dp, "test", "value");
    if ((int) &val == -1) { clearmem(); return (-1); }
    printf("Value set to: value\n");

    status = removeTuple(dp, "test");
    if (status != 0) { clearmem(); return (-1); }
    printf("Value removed: test\n");

    /* should return -1 (nothing is there) */
    val = getTuple(dp, "test");
    if ((int) &val == -1) { clearmem(); return (-1); }
    printf("Value received: %i\n", (int) val);

    status = setTuple(dp, "test2", (void *) 1);
    if (status != 0) { clearmem(); return (-1); }
    printf("Value set to: test2\n");

    val = getTuple(dp, "foo");
    if ((int) &val == -1) { clearmem(); return (-1); }
    printf("Value received: %s\n", (char *) val);

    destroyDict(dp);
    return(0);
}

double calcExecTime(struct timeval tv1, struct timeval tv2) {
    double time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +\
                  (double) (tv2.tv_sec - tv1.tv_sec);
    return time;
}

/***************** Error Handlng / Stack Trace ****************************/

void posix_print_stack_trace() {
    char *prog_name = getenv("PWD");
    int i, trace_size = 0;
    char **messages = (char **)NULL;

    trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
    messages = backtrace_symbols(stack_traces, trace_size);

    /* skip the first couple stack frames (as they are this function and
       our handler) and also skip the last frame as it's (always?) junk. */
    /* for (i = 3; i < (trace_size - 1); ++i) we'll use this so you can see what's going on */
    for (i = 0; i < trace_size; ++i) {
        if (addr2line(prog_name, stack_traces[i]) != 0) {
            printf("  error determining line # for: %s\n", messages[i]);
        }

    }   if (messages) { free(messages); }
}

/* Resolve symbol name and source location given the path to the executable and an address */
int addr2line(char const * const PROG_NAME, void const * const addr) {
    char addr2line_cmd[512] = {0};

    /* have addr2line map the address to the relent line in the code */
#ifdef __APPLE__
    /* apple does things differently... */
    sprintf(addr2line_cmd,"atos -o %.256s %p", program_name, addr);
#else
    sprintf(addr2line_cmd,"addr2line -f -p -e %.256s %p", PROG_NAME, addr);
#endif

    /* This will print a nicely formatted string specifying the function and source line of the address */
    return system(addr2line_cmd);
}


void sig_handler(int signo, siginfo_t *siginfo, void *context) {
    (void)context;
    switch(signo) {
        case SIGHUP:
            perror("\nSIGHUP");
            exit(-2);
        case SIGINT:
            perror("\nSIGINT");
            exit(-3);
        case SIGQUIT:
            perror("\nSIGQUIT");
            exit(-4);
        case SIGSEGV:
            perror("\nSIGSEGV");
            exit(-5);
        case SIGABRT:
            perror("\nSIGABRT");
            exit(-6);
        case SIGFPE:
            switch(siginfo->si_code) {
                case FPE_INTDIV:
                    fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
                    break;
                case FPE_INTOVF:
                    fputs("Caught SIGFPE: (integer overflow)\n", stderr);
                    break;
                case FPE_FLTDIV:
                    fputs("Caught SIGFPE: (floating-point divide by zero)\n", stderr);
                    break;
                case FPE_FLTOVF:
                    fputs("Caught SIGFPE: (floating-point overflow)\n", stderr);
                    break;
                case FPE_FLTUND:
                    fputs("Caught SIGFPE: (floating-point underflow)\n", stderr);
                    break;
                case FPE_FLTRES:
                    fputs("Caught SIGFPE: (floating-point inexact result)\n", stderr);
                    break;
                case FPE_FLTINV:
                    fputs("Caught SIGFPE: (floating-point invalid operation)\n", stderr);
                    break;
                case FPE_FLTSUB:
                    fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
                    break;
                default:
                    fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
            }
            perror("\nSIGFPE");
            exit(-7);
        case SIGILL:
            switch(siginfo->si_code) {
                case ILL_ILLOPC:
                    fputs("Caught SIGILL: (illegal opcode)\n", stderr);
                    break;
                case ILL_ILLOPN:
                    fputs("Caught SIGILL: (illegal operand)\n", stderr);
                    break;
                case ILL_ILLADR:
                    fputs("Caught SIGILL: (illegal addressing mode)\n", stderr);
                    break;
                case ILL_ILLTRP:
                    fputs("Caught SIGILL: (illegal trap)\n", stderr);
                    break;
                case ILL_PRVOPC:
                    fputs("Caught SIGILL: (privileged opcode)\n", stderr);
                    break;
                case ILL_PRVREG:
                    fputs("Caught SIGILL: (privileged register)\n", stderr);
                    break;
                case ILL_COPROC:
                    fputs("Caught SIGILL: (coprocessor error)\n", stderr);
                    break;
                case ILL_BADSTK:
                    fputs("Caught SIGILL: (internal stack error)\n", stderr);
                    break;
                default:
                    fputs("Caught SIGILL: Illegal Instruction\n", stderr);
            }
            perror("\nSIGILL");
            exit(-8);
        case SIGTERM:
            perror("\nSIGTERM");
            exit(-9);
        default:
            perror("\nUnknown signal caught");
    }
    posix_print_stack_trace();
    _Exit(1);
}

void set_sig_handler() {
    { /*           setup alternate stack          */
        stack_t ss = {};
        /* malloc is usually used here, I'm not 100% sure my static allocation
           is valid but it seems to work just fine. */
        ss.ss_sp = (void *) alternate_stack;
        ss.ss_size = SIGSTKSZ;
        ss.ss_flags = 0;

        if (sigaltstack(&ss, NULL) != 0) { err(1, "sigaltstack"); }
    }

    { /*       register our signal handlers      */
        struct sigaction sig_action = {};
        sig_action.sa_sigaction = sig_handler;
        sigemptyset(&sig_action.sa_mask);

#ifdef __APPLE__
        /* for some reason we backtrace() doesn't work on osx when we use an alternate stack */
        sig_action.sa_flags = SA_SIGINFO;
#else
        sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
#endif

        if (
                (sigaction(SIGHUP, &sig_action, NULL) != 0)     ||
                (sigaction(SIGINT, &sig_action, NULL) != 0)     ||
                (sigaction(SIGQUIT, &sig_action, NULL) != 0)    ||
                (sigaction(SIGSEGV, &sig_action, NULL) != 0)    ||
                (sigaction(SIGABRT, &sig_action, NULL) != 0)    ||
                (sigaction(SIGFPE, &sig_action, NULL) != 0)     ||
                (sigaction(SIGILL, &sig_action, NULL) != 0)     ||
                (sigaction(SIGTERM, &sig_action, NULL) != 0)
                ) { perror("Can't set signal handler\n"); err(1, "sigaction"); }
    }
}