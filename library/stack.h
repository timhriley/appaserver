/* stack.h */
/* ------- */

#ifndef STACK_H
#define STACK_H

#include "boolean.h"

/* ========================= */
/* Header file for stack ADT */
/* ========================= */
#define MAX_STACK	1000

typedef struct {
	char *array[ MAX_STACK ];
        int num_in_array;
} STACK;


/* STACK Function Prototypes */
/* ------------------------- */
STACK *create_stack();
STACK *stack_new();
boolean stack_push( STACK *s, void *item );
boolean push( STACK *s, void *item );
int num_in_stack( STACK *s );
int stack_size( STACK *s );
void *pop( STACK *s );
void *stack_pop( STACK *s );
void *top( STACK *s );
void *stack_top( STACK *s );
void *bottom( STACK *s );
void *stack_bottom( STACK *s );
void destroy_stack( STACK *s );
void stack_free( STACK *s );
void stack_reset( STACK *s );

#endif
