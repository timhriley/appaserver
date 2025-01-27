/* stack.c */
/* ------- */

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

STACK *stack_new()
{
	return create_stack();
}

STACK *create_stack()
{
	STACK *s = (STACK *)calloc( 1, sizeof( STACK ) );
	if ( !s )
	{
		fprintf(stderr,
			"create_stack() cannot allocate memory\n" );
		exit( 1 );
	}
	return s;
}

int stack_size( STACK *s )
{
	return num_in_stack( s );
}

int num_in_stack( STACK *s )
{
	return s->num_in_array;
}

boolean stack_push( STACK *s, void *item )
{
	return push( s, item );
}

boolean push( STACK *s, void *item )
{
	if ( s->num_in_array == MAX_STACK ) return 0;

	s->array[ s->num_in_array ] = item;
	s->num_in_array++;
	return 1;

} /* push() */

void *stack_pop( STACK *s )
{
	return pop( s );
}

void *pop( STACK *s )
{
	if ( !s->num_in_array )
	{
		return (char *)0;
	}

	(s->num_in_array)--;
	return s->array[ s->num_in_array ];
}


void *stack_top( STACK *s )
{
	return top( s );
}

void *top( STACK *s )
{
	if ( !s->num_in_array )
	{
		return (char *)0;
	}
	return s->array[ s->num_in_array - 1 ];
}

void *stack_bottom( STACK *s )
{
	return bottom( s );
}

void *bottom( STACK *s )
{
	if ( !s->num_in_array )
	{
		return (char *)0;
	}
	return s->array[ s->num_in_array  - 1 ];
}

void stack_free( STACK *s )
{
	destroy_stack( s );
}

void destroy_stack( STACK *s )
{
	free( s );
}

void stack_reset( STACK *s )
{
	s->num_in_array = 0;
}

