/* menutree.c */
/* ---------- */
/* --------------------------------------------------------
	This module supports a tree structured menu system.

	Tim Riley
----------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "piece.h"
#include "stack.h"
#include "list.h"
#include "queue.h"
#include "column.h"
#include "timlib.h"
#include "environ.h"
#include "menutree.h"

/* Array of functions */
/* ------------------ */
static struct
{
	char *fn_name;
	int (*fn)();
} fn_array[] = {{"executable",			executable},
		{"list_values",			list_values},
		{"confirm",			confirm},
		{"directory",			directory},
		{"prompt",			prompt},
		{"text",			text},
		{"save",			save},
		{(char *)0,			NULL }};

MENU_TREE *menu_tree_init( void )
{
	MENU_TREE *m = (MENU_TREE *)calloc( 1, sizeof( MENU_TREE ) );

	m->node_stack = create_stack();

	/* Create the root node */
	/* -------------------- */
	m->root.display_text = "Menu";
	m->root.node_list = create_list();
	m->root.text_queue = queue_init();
	push( m->node_stack, (char *)&m->root );
	m->current = (NODE *)top( m->node_stack );

	return m;

} /* menu_tree_init() */

void interact( MENU_TREE *m )
{
	char response[ 64 ];
	int response_int;
	NODE *save_node;

	/* Exit in the middle. */
	/* ------------------- */
	while( 1 )
	{
		menu_tree_display_root_text( m->current->display_text );

		menu_tree_display_level( m );

		printf( "\nEnter: " );
		get_line( response, stdin );

		/* Get response */
		/* ------------ */
		response_int = atoi( response );

		/* Must be a positive integer */
		/* -------------------------- */
		if ( response_int < 0 ) continue;

		if ( response_int == 99 )
		{
			/* Pop the previous menu */
			/* --------------------- */
			if ( !menu_tree_pop( m ) ) 
			{
				/* ------------------------- */
				/* If entered 99 at the root */
				/* ------------------------- */
				return;
			}
		}

		save_node = m->current;

		/* menu_tree_goto_selection() alters m->currrent */
		/* --------------------------------------------- */
		if ( menu_tree_goto_selection( m, response_int ) )
		{
			/* Check if executable */
			/* ------------------- */
			if ( m->current->executable )
			{
				printf( 
"=======================================================================\n" );
				printf( "Selected:\t%s\n", 
					 m->current->display_text );
				printf( "Executing:\t%s\n", 
					 m->current->executable );
				if ( m->current->directory )
				{
					printf( "Directory:\t%s\n", 
						m->current->directory );
				}

				display_text_queue( m->current );

				if ( m->current->save_key )
				{
					load_default_values( 
							m->current->prompt_list,
						     	m->current->save_key );
				}

				/* Display the prompts */
				/* ------------------- */
				display_prompts( m->current->prompt_list );

				if ( menu_tree_confirm( m->current ) )
				{
					char nothing[ 64 ];
					execute_it( m->current );
					printf( "\nPress <Return> " );
					get_line( nothing, stdin );
					printf( "\n" );
				}

				m->current = save_node;
			}
			else
			{
				menu_tree_push( m );
			}
		}
	}

} /* interact() */


void menu_tree_display_root_text( char *display_text )
{
	printf( "\n" );
	printf( "%s\n", display_text );
	printf( 
"-------------------------------------------------------------------------\n" );

} /* menu_tree_display_root_text() */


void menu_tree_goto_root( MENU_TREE *m )
{
	m->current = (NODE *)top( m->node_stack );
}


int menu_tree_load_recursive( MENU_TREE *m, FILE *infile, NODE *root )
{
	char process_string[ MAX_STRING ];
	char piece_buffer[ MAX_STRING ];

	while( timlib_get_line_escape_CR(
			process_string,
			infile,
			MAX_STRING ) )
	{
		trim( process_string );

		/* Ignore blank lines and comments */
		/* ------------------------------- */
		if ( !*process_string || *process_string == '#' ) continue;

		/* If all finished at this level */
		/* ----------------------------- */
		if ( *process_string == ')' )
		{
			return 1;
		}
		else
		/* If new level */
		/* ------------ */
		if ( *process_string == '(' )
		{
			NODE *node_ptr;

			/* The last node now becomes the root */
			/* ---------------------------------- */
			if ( !go_tail( root->node_list ) )
			{
				fprintf( stderr,
				"Invalid file structure with line (%s)\n",
					 process_string );
				exit( 1 );
			}

			node_ptr = (NODE *)retrieve_item_ptr( root->node_list );
			node_ptr->node_list = create_list();

			menu_tree_load_recursive(	m, 
							infile, 
							node_ptr );
		}
		else
		/* ---------- */
		/* Same level */
		/* ---------- */
		{
			NODE new_node;

			/* Zap it */
			/* ------ */
			memset( &new_node, '\0', sizeof( NODE ) );

			/* Initialize the prompt list */
			/* -------------------------- */
			new_node.prompt_list = create_list();

			/* Set the text */
			/* ------------ */
			new_node.display_text = strdup( piece( 	piece_buffer,
							'[',
							process_string,
							0 ) );

			/* If there exists other parameters */
			/* -------------------------------- */
			if ( piece( piece_buffer, '[', process_string, 1 ) )
			{
				/* Initialize the text queue */
				/* ------------------------- */
				new_node.text_queue = queue_init();

				set_other_node_info(	&new_node, 
							process_string );
			}


			/* Add this menu item */
			/* ------------------ */
			add_item(	root->node_list, 
					&new_node, 
					sizeof( NODE ),
					ADD_TAIL );
		}
	}

	return 1;

} /* menu_tree_load_recursive() */


int set_other_node_info( NODE *node_ptr, char *process_string )
{
	char piece_buffer[ MAX_STRING ];
	char column_buffer[ MAX_STRING ];
	char argument_string[ MAX_STRING ];
	char main_string[ MAX_STRING ];
	char command_string[ 128 ];
	int i, fn_offset;

	/* ------------------------------------------------------------ */
	/* Sample:						        */
	/* [executable=item1.sh prompt="Enter sponsor key"=sponsor_key] */
	/* ------------------------------------------------------------ */
	piece( piece_buffer, '[', process_string, 1 );
	piece( main_string, ']', piece_buffer, 0 );

	/* ---------------------------------------------------------- */
	/* New Sample:						      */
	/* executable=item1.sh prompt="Enter sponsor key"=sponsor_key */
	/* ---------------------------------------------------------- */

	/* --------------- */
	/* For each column */
	/* --------------- */
	for( 	i = 0; 
		column( column_buffer, i, main_string ); 
		i++ )
	{
		piece( command_string, '=', column_buffer, 0 );

		/* Search table for function */
		/* ------------------------- */
		fn_offset = search_fn_array( command_string );
	
		/* If not in table */
		/* --------------- */
		if ( fn_offset == -1 )
		{
			fprintf( stderr, 
				 "Invalid command: (%s) in (%s)\n",
				 command_string,
				 process_string );

			exit( 1 );
		}
	
		/* -------------------------------------- */
		/* New Sample:				  */
		/* prompt="Enter sponsor key"=sponsor_key */
		/* -------------------------------------- */
		piece_split_in_two( argument_string, '=', column_buffer, 1 );

		/* Execute the function and pass the argument line */
		/* ----------------------------------------------- */
		fn_array[ fn_offset ].fn( node_ptr, argument_string );
	}

	return 1;

} /* set_other_node_info() */


int search_fn_array( char *command )
{
	int i;

	for( i = 0; fn_array[ i ].fn_name; i++ )
	{
		if( strncmp(	fn_array[ i ].fn_name, 
				command, 
				strlen( fn_array[ i ].fn_name ) ) == 0 )
		{
			return i;
		}
	}
	return -1;

} /* search_fn_array() */


int prompt( NODE *node_ptr, char *argument_string )
{
	char piece_buffer[ MAX_STRING ];
	PROMPT *p_ptr;
	PROMPT p;

	p.prompt = strdup( piece(	piece_buffer, 
					'=', 
					argument_string, 
					0 ) );

	p.variable = strdup( piece(	piece_buffer, 
					'=', 
					argument_string, 
					1 ) );

	if ( !p.variable )
	{
		fprintf( stderr, 
			 "prompt() has invalid command (%s)\n",
			 argument_string );
		exit( 1 );
	}

	p_ptr = (PROMPT *)add_item(	node_ptr->prompt_list, 
					&p, 
					sizeof( PROMPT ),
					ADD_TAIL );

	/* Set current_prompt */
	/* ------------------ */
	node_ptr->current_prompt = p_ptr;

	return 1;
} /* prompt() */

int executable( NODE *node_ptr, char *argument_string )
{
	char *executable = strdup( argument_string );

	if ( !executable )
	{
		fprintf( stderr, 
			"executable() cannot get executable in (%s)\n",
			 argument_string );
		exit( 1 );
	}
	else
	{
		node_ptr->executable = executable;
	}
	return 1;
} /* executable() */


int list_values( NODE *node_ptr, char *argument_string )
{
	node_ptr->current_prompt->list_values = strdup( argument_string );
	return 1;

} /* list_values() */


int confirm( NODE *node_ptr, char *argument_string )
{
	if ( toupper( *argument_string ) != 'Y' )
	{
		fprintf( stderr, 
			 "confirm() cannot get confirm in (%s)\n",
			 argument_string );
		exit( 1 );
	}
	else
	{
		node_ptr->confirm = 1;
	}
	return 1;

} /* confirm() */


int directory( NODE *node_ptr, char *argument_string )
{
	node_ptr->directory = strdup( argument_string );
	return 1;

} /* directory() */


int text( NODE *node_ptr, char *argument_string )
{
	char *text = strdup( argument_string );
	if ( !text )
	{
		fprintf( stderr, 
			 "text() cannot get text in (%s)\n",
			 argument_string );
		exit( 1 );
	}
	else
		enqueue( node_ptr->text_queue, text );

	return 1;

} /* text() */

int save( NODE *node_ptr, char *argument_string )
{
	node_ptr->save_key = strdup( argument_string );
	return 1;
} /* save() */


int menu_tree_load( MENU_TREE *m, char *filename )
{
	FILE *infile;

	infile = fopen( filename, "r" );
	if ( !infile )
	{
		char buffer[ 128 ];

		sprintf( buffer, "File open error: (%s)", filename );
		return menu_tree_handle_error( m, buffer );
	}

	/* Point to root */
	/* ------------- */
	menu_tree_goto_root( m );

	/* Do the load */
	/* ----------- */
	menu_tree_load_recursive( m, infile, m->current );

	return 1;

} /* menu_tree_load() */


char *menu_tree_get_error( MENU_TREE *m )
{
	return m->error_buffer;
}


int menu_tree_handle_error( MENU_TREE *m, char *error )
{
	strcpy( m->error_buffer, error );	
	return -1;
} /* menu_tree_handle_error() */


void menu_tree_display_stack( MENU_TREE *m )
{
	int i;
	NODE *n;

	printf( "Stack\n" );
	printf( "-----\n" );
	for ( i = 0; i < m->node_stack->num_in_array; i++ )
	{
		n = (NODE*) m->node_stack->array[ i ];

		if ( n->executable )
			printf( "*%s\n", n->display_text );
		else
			printf( "%s\n", n->display_text );
	}
	printf( "\n" );
}

void menu_tree_display_level( MENU_TREE *m )
{
	NODE *node;
	int i = 0;

	if ( go_head( m->current->node_list ) )
	{
		do
		{
			node = 
			(NODE *)retrieve_item_ptr( m->current->node_list );

			if ( node->executable )
				printf( "%.2d) *%s\n", 
					 ++i,
					 node->display_text );
			else
				printf( "%.2d) %s\n", 
					++i, 
					node->display_text );

		} while( next_item( m->current->node_list ) );

		/* If at root then display 99) Exit */
		/* -------------------------------- */
		if ( m->current == &m->root )
			printf( "99) Exit\n" );
		else
			printf( "99) Back\n" );
	}
}

NODE *menu_tree_pop( MENU_TREE *m )
{
	NODE *n;

	if ( ( n = (NODE *)pop( m->node_stack ) ) ){};
	m->current = (NODE *)top( m->node_stack );
	return m->current;
}

void menu_tree_push( MENU_TREE *m )
{
	push( m->node_stack, (char *)m->current );
}

void menu_tree_free( MENU_TREE *m )
{
	destroy_list( m->root.node_list );
	/* queue_free( m->root.text_queue ); */
	destroy_stack( m->node_stack );
	free( m );
}

int menu_tree_goto_selection( MENU_TREE *m, int i )
{
	NODE *n;

	if ( !go_head( m->current->node_list ) )
		return 0;

	do
	{
		if ( i-- == 1 )
		{
			n = (NODE *)retrieve_item_ptr( m->current->node_list );

			/* If menu.dat has nothing between the "(" and ")" */
			/* ----------------------------------------------- */
			if ( !n->executable && !go_head( n->node_list ) )
			{
				printf( "\nEmpty!\n\n" );
				return 0;
			}

			m->current = n;
			return 1;
		}

	} while( next_item( m->current->node_list ) );

	return 0;

} /* menu_tree_goto_selection() */


void display_text_queue( NODE *n )
{
	if ( !queue_is_empty( n->text_queue ) )
	{
		printf( "\n" );
		printf( "===========\n" );
		printf( "Information\n" );
		printf( "===========\n" );
		queue_display( n->text_queue );
		printf( "\n" );
	}

} /* display_text_queue() */


int execute_it( NODE *n )
{
	char sys_str[ 512 ];

	get_variables_from_user( n->prompt_list );
	populate_sys_str( sys_str, n->prompt_list, n->executable );

	change_the_directory( n );

	if ( n->save_key )
		save_default_values( n->prompt_list, n->save_key );
	return system( sys_str );

} /* execute_it() */



int menu_tree_confirm( NODE *n )
{
	if ( n->confirm )
	{
		char buffer[ 64 ];
		printf( "\nConfirm continue <Y> " );
		get_line( buffer, stdin );

		return ( toupper( *buffer ) == 'Y' );
	}
	else
	{
		return 1;
	}

} /* menu_tree_confirm() */


void display_prompts( LIST *prompt_list )
{
	PROMPT *p;

	if ( go_head( prompt_list ) )
	{
		printf( "\n" );
		printf( "The prompts are:\n" );
		do
		{
			p = (PROMPT *)retrieve_item_ptr( prompt_list );

			if ( p->default_value && *p->default_value )
			{
				printf( "\t%s (Enter=%s)\n", 
					p->prompt, 
					p->default_value );
			}
			else
				printf( "\t%s\n", p->prompt );

		} while( next_item( prompt_list ) );
		printf( "\n" );
	}

} /* display_prompts() */

int change_the_directory( NODE *n )
{
	char dir_string[ MAX_STRING ];

	if ( n->directory )
	{
		return chdir( resolve_environment_variables(
				dir_string,
				n->directory ) );
	}
	
	return 1;

} /* change_the_directory() */


char *replace_sys_str_with_prompt_list( char *s, LIST *prompt_list )
{
	PROMPT *p;
	struct LINKTYPE *current_record;

	current_record = get_current_record( prompt_list );

	if ( go_head( prompt_list ) )
	{
		do
		{
			p = (PROMPT *)retrieve_item_ptr( prompt_list );
			search_replace_single_quoted_string(
					s,
					p->variable, 
					p->entered_value );

		} while( next_item( prompt_list ) );
	}

	go_record( prompt_list, current_record );

	return s;

} /* replace_sys_str_with_prompt_list() */



void populate_sys_str( char *sys_str, LIST *prompt_list, char *executable )
{
	PROMPT *p;

	strcpy( sys_str, executable );

	if ( go_head( prompt_list ) )
	{
		do
		{
			p = (PROMPT *)retrieve_item_ptr( prompt_list );
			search_replace_single_quoted_string(
					sys_str,
					p->variable, 
					p->entered_value );
/*
			search_replace(	p->variable, 
					p->entered_value, 
					sys_str );
*/
		} while( next_item( prompt_list ) );
	}

} /* populate_sys_str() */

void get_variables_from_user( LIST *prompt_list )
{
	PROMPT *p;
	char buffer[ 1024 ];

	if ( go_head( prompt_list ) )
	{
		do
		{
			p = (PROMPT *)retrieve_item_ptr( prompt_list );

			/* Execute a list values (maybe) */
			/* ----------------------------- */
			if ( p->list_values )
			{
				char sys_str[ 1024 ];

				printf( "\n" );
				strcpy( sys_str, p->list_values );

				if ( system( replace_sys_str_with_prompt_list( 
					sys_str, prompt_list ) ) ){}

				printf( "\n" );
			}

			if ( p->default_value && *p->default_value )
			{
				printf( "%s (Enter=%s): ", 
					p->prompt,
					p->default_value );
			}
			else
				printf( "%s: ", p->prompt );

			*buffer = '\0';
			get_line( buffer, stdin );

			if ( !*buffer && p->default_value )
			{
				strcpy( buffer, p->default_value );
			}

			/* Save the entry */
			/* -------------- */
			if ( p->entered_value ) free( p->entered_value );
			p->entered_value = strdup( buffer );

		} while( next_item( prompt_list ) );
	}

} /* get_variables_from_user() */


void save_default_values( LIST *prompt_list, char *save_key )
{
	char *whoami = get_system( "whoami" );
	char buffer[ 256 ];
	PROMPT *p;
	FILE *f;

	if ( !save_key ) return;

	sprintf( buffer, SAVE_FILE_PATTERN, whoami, save_key );

	f = fopen( buffer, "w" );
	if ( !f ) return;

	if ( !go_head( prompt_list ) ) return;

	do
	{
		p = (PROMPT *)retrieve_item_ptr( prompt_list );

		fprintf( f, "%s=%s\n", p->variable, p->entered_value );

	} while( next_item( prompt_list ) );
	
	fclose( f );

} /* save_default_values() */

void load_default_values( LIST *prompt_list, char *save_key )
{
	char *whoami = get_system( "whoami" );
	char buffer[ 256 ];
	char variable[ 256 ];
	char default_value[ 256 ];
	PROMPT *p;
	FILE *f;

	if ( !save_key ) return;

	sprintf( buffer, SAVE_FILE_PATTERN, whoami, save_key );

	f = fopen( buffer, "r" );
	if ( !f ) return;

	while( timlib_get_line_escape_CR( buffer, f, 256 ) )
	{
		piece( variable, '=', buffer, 0 );

		if ( !piece( default_value, '=', buffer, 1 ) ) continue;

		if ( !( p = prompt_search( prompt_list, variable ) ) )
			continue;

		if ( p->default_value ) free( p->default_value );
		p->default_value = strdup( default_value );
	}

	fclose( f );

} /* load_default_values() */

PROMPT *prompt_search( LIST *prompt_list, char *variable )
{
	PROMPT *p;

	if ( !go_head( prompt_list ) ) return (PROMPT *)0;

	do
	{
		p = (PROMPT *)retrieve_item_ptr( prompt_list );
		if ( strcmp( variable, p->variable ) == 0 ) return p;

	} while( next_item( prompt_list ) );

	return (PROMPT *)0;

} /* prompt_search() */

