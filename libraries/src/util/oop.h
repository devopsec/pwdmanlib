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

#ifndef PWDMANLIB_OOP_H
#define PWDMANLIB_OOP_H

/********************************************************************************
 *             Utility classes and functions for OOP operations                 *
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/******************               HASH MAP               ************************/

// pre-declarations of structured types to key-val map
typedef struct hash_map *hash_map_t;
// key-value pair checking
typedef int (*hash_map_validator_t)(const void *key1, const void *key2, void *data);
// constructor
hash_map_t create_map (hash_map_validator_t compare_keys, void *data);
// destructor
void destroy_map (hash_map_t map);
// setter
void set_map (hash_map_t map, const void *key, const void *value);
// getter
void *get_map (hash_map_t map, void *key);
// de-referencing
void *remove_map (hash_map_t map, void *key);

/*******************           Virtual Class             ************************/

// TODO: sort through this and see if there's anything useful

// The top-level class.

typedef struct _tCommClass {
    int (*open)(struct _tCommClass *self, char *fspec);
} tCommClass;

// Function for the TCP 'class'.

static int tcpOpen (tCommClass *tcp, char *fspec) {
    printf ("Opening TCP: %s\n", fspec);
    return 0;
}
static int tcpInit (tCommClass *tcp) {
    tcp->open = &tcpOpen;
    return 0;
}

// Function for the HTTP 'class'.

static int httpOpen (tCommClass *http, char *fspec) {
    printf ("Opening HTTP: %s\n", fspec);
    return 0;
}
static int httpInit (tCommClass *http) {
    http->open = &httpOpen;
    return 0;
}

/*******************              Modules                ************************/

typedef struct Module_struct Module;          /* module declaration */
struct Module_struct {
    int element;
};
Module *Module_create();                      /* constructor */
void    Module_delete( Module * );            /* destructor */
int     Module_get_element( const Module * ); /* accessor */
void    Module_set_element( Module *, int );  /* modifier */

/******************         Dictionary (static-size)     ************************/

typedef struct tuple Tuple;
typedef struct dict Dict;
int findKey(Dict *d, char *check);

struct tuple {
    char                   *key;           //named loc
    void                   *val;           //data
};

struct dict {
    unsigned int           size;           //max size
    unsigned int           counter;        //# tuples
    Tuple                  *table;         //table of tuples
    Tuple                  **entries;      //arr of ptrs
};

/* TODO: use this null initializer instead */
const Tuple EmptyTuple = (const Tuple){'\0'};

Dict *initDict(const unsigned int size) {
    Dict *d = (Dict *) malloc(sizeof(Dict));
    if (d) {
        d->size = size;
        d->counter = 0;
        d->table = malloc(sizeof(Tuple) * size);
        d->entries = malloc(sizeof(Tuple*) * size);

        if (d->table && d->entries) {
            for (unsigned int i = 0; i < size; ++i) {
                d->entries[i] = d->table + i;
                d->entries[i]->key = '\0';  /* init as null */
            }
            return d;
        }      /*       Failed to init:      */
        else { /* Data must be cleaned here. */
            free(d->entries);
            free(d->table);
            free(d);
        } exit(-1); /* fall through */
    } exit(-1);  /* fail safe */
}

short setTuple(Dict *d, char *key, void* val) {
    int i;
    if ((i = findKey(d, key)) != -1) { /* found key */
        d->entries[i]->val = val;
        return 0;
    }
    else {  /* new entry */
        if (d->counter < d->size) { /* find first null */
            unsigned int i = 0;     /*   reset   */
            for (i; i < d->size; i++) {
                if (! d->entries[i] || ! d->entries[i]->key) break;
            } /* then replace null slot */
            d->entries[i]->key = key;
            d->entries[i]->val = val;
            d->counter++;
            return 0;
        } /* no room left in dict */
    } return -1;
}

/* TODO: parse json into dict */
short setTuples(Dict *d, char *json) {
    exit(-1);
}

void* getTuple(Dict *d, char *key) {
    int i;
    if ((i = findKey(d, key)) != -1) {
        void *val = d->entries[i]->val;
        return val;
    } return (void *) -1; /* not found */
}

void* getIndex(Dict *d, unsigned int index) {
    if (index < d->counter && d->entries[index]->key != NULL) {
        void *val = d->entries[index]->val;
        return val;
    } return (void *) -1; /* not found */
}

/* TODO: add checks for NULL ptr prior to freeing? */
int removeTuple(Dict *d, char *key) {
    int i;
    if ((i = findKey(d, key)) != -1) {
        d->entries[i]->key = '\0';
        d->entries[i]->val = '\0';
        d->counter--;
        return 0;
    } return -1; /* no room */
}

void destroyDict(Dict *d) {
    for (unsigned int i = 0; i < d->counter; i++) {
        d->entries[i] = '\0';
        d->counter--;
    }
    free(d->entries);
    free(d->table);
    free(d);
}

/* return index of tuple in dict or -1 if DNE */
int findKey(Dict *d, char *check) {
    unsigned int i;
    for (i = 0; i < d->counter; i++) {
        if (d->entries[i]->key != NULL && check != NULL && strcmp(d->entries[i]->key, check) == 0) {
            return i;
        }
    } return -1; /* not found */
}

/******************      Dynamic Method Dispatching      ************************/

/*
 * When a user of a stack invokes a stack operation on the stack instance, the operation will
 * dispatch to the corresponding operation in the vtable. This vtable is initialized by the
 * creation function with the functions that correspond to its particular implementation.
 */
#define STACK_ARRAY_MAX 10000

struct Stack {
    const struct StackInterface * const vtable;
};

struct StackInterface {
    int (*top)(struct Stack *);
    void (*pop)(struct Stack *);
    void (*push)(struct Stack *, int);
    int (*empty)(struct Stack *);
    int (*full)(struct Stack *);
    void (*destroy)(struct Stack *);
};

inline int stack_top (struct Stack *s) { return s->vtable->top(s); }
inline void stack_pop (struct Stack *s) { s->vtable->pop(s); }
inline void stack_push (struct Stack *s, int x) { s->vtable->push(s, x); }
inline int stack_empty (struct Stack *s) { return s->vtable->empty(s); }
inline int stack_full (struct Stack *s) { return s->vtable->full(s); }
inline void stack_destroy (struct Stack *s) { s->vtable->destroy(s); }

//implement stack with fixed-size array
struct StackArray {
    struct Stack base;
    int idx;
    int array[STACK_ARRAY_MAX];
};
static int stack_array_top (struct Stack *s) { /* ... */ }
static void stack_array_pop (struct Stack *s) { /* ... */ }
static void stack_array_push (struct Stack *s, int x) { /* ... */ }
static int stack_array_empty (struct Stack *s) { /* ... */ }
static int stack_array_full (struct Stack *s) { /* ... */ }
static void stack_array_destroy (struct Stack *s) { /* ... */ }
struct Stack * stack_array_create () {
    static const struct StackInterface vtable = {
            stack_array_top, stack_array_pop, stack_array_push,
            stack_array_empty, stack_array_full, stack_array_destroy
    };
    static struct Stack base = { &vtable };
    struct StackArray *sa = malloc(sizeof(*sa));
    memcpy(&sa->base, &base, sizeof(base));
    sa->idx = 0;
    return &sa->base;
}

//implement stack using list
struct StackList {
    struct Stack base;
    struct StackNode *head;
};
struct StackNode {
    struct StackNode *next;
    int data;
};
static int stack_list_top (struct Stack *s) { /* ... */ }
static void stack_list_pop (struct Stack *s) { /* ... */ }
static void stack_list_push (struct Stack *s, int x) { /* ... */ }
static int stack_list_empty (struct Stack *s) { /* ... */ }
static int stack_list_full (struct Stack *s) { /* ... */ }
static void stack_list_destroy (struct Stack *s) { /* ... */ }
struct Stack * stack_list_create () {
    static const struct StackInterface vtable = {
            stack_list_top, stack_list_pop, stack_list_push,
            stack_list_empty, stack_list_full, stack_list_destroy
    };
    static struct Stack base = { &vtable };
    struct StackList *sl = malloc(sizeof(*sl));
    memcpy(&sl->base, &base, sizeof(base));
    sl->head = 0;
    return &sl->base;
}

#endif //PWDMANLIB_OOP_H