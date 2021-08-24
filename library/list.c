/* $APPASERVER_HOME/library/list.c			   */
/* ======================================================= */
/* Function code for linked list package                   */
/* It supports the LIST ADT.              		   */
/*                                                         */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "String.h"
#include "dictionary.h"
#include "list.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"

/* ------------------------------- */
/* Free the data but keep the list */
/* ------------------------------- */
void list_free_data( LIST *list )
{
	void *data;

	if ( list_rewind( list ) )
	{
		do {
			data = list_get_pointer( list );
			free( data );
		} while( list_next( list ) );
	}
}

/* ------------------------------- */
/* Free the list but keep the data */
/* ------------------------------- */
int list_free_container( LIST *list )
{
	struct LINKTYPE *linktype;

	if ( list_rewind( list ) )
	{
		while( list->current )
		{
			linktype = list->current;
			list->current = list->current->next;
			free( linktype );
		}
	}

	free( list );
	return 1;
}

void list_linktype_free( struct LINKTYPE *linktype )
{
	if ( linktype && linktype->item ) free( linktype->item );
	if ( linktype ) free( linktype );
}

int list_free_string_container( LIST *list )
{
	return list_free_container( list );
}

boolean list_set( LIST *list, void *this_item )
{
        struct LINKTYPE *newlink;

	if ( !list ) return 0;
	if ( !this_item ) return 0;

        if ( ! ( newlink = create_node() ) ) return 0;

        newlink->item = this_item;
        newlink->num_bytes = -1;

        newlink->previous = list->tail->previous;
        newlink->next = list->tail;
        newlink->previous->next = newlink;
        list->tail->previous = newlink;

        list->num_in_list++;
	list->current = newlink;

	return 1;
}

boolean list_set_pointer( LIST *list, void *this_item )
{
	return list_set( list, this_item );
}

boolean list_append_pointer( LIST *list, void *this_item )
{
	return list_set( list, this_item );
}

boolean list_set_first( LIST *list, void *this_item )
{
	return list_prepend_pointer( list, this_item );
}

boolean list_add_head( LIST *list, void *this_item )
{
	return list_prepend_pointer( list, this_item );
}

boolean list_prepend_pointer( LIST *list, void *this_item )
{
        struct LINKTYPE *newlink = create_node();

	if ( !list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): list is null\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

        if (!newlink) return 0;

        newlink->item = this_item;
        newlink->num_bytes = -1;

        list->num_in_list++;
	list->current = newlink;

	newlink->next = list->head->next;
	newlink->previous = list->head;
	newlink->next->previous = newlink;
	list->head->next = newlink;

	return 1;

}

void list_append_string_list( LIST *list, LIST *string_list )
{
	char *item;

	if ( !list ) return;

	if ( list_reset( string_list ) )
	{
		do {
			item = list_get_pointer( string_list );
			list_append_pointer( list, item );
		} while( list_next( string_list ) );
	}

}

void list_subtract_string(
			LIST *list,
			char *string )
{
	if ( item_exists( list, string, list_strcmp ) ) delete_current( list );
}


int list_delete_current( LIST *list )
{
	return delete_current( list );
}

void *list_get_first_element( LIST *list )
{
	return list_get_first( list );
}

void *list_get_first_pointer( LIST *list )
{
	return list_get_first( list );
}

void *list_data( LIST *list )
{
	return list_get_pointer( list );
}

void *list_last( LIST *list )
{
	return list_get_last_pointer( list );
}

void *list_get_last( LIST *list )
{
	return list_get_last_pointer( list );
}

void *list_get_last_pointer( LIST *list )
{
	if ( !list ) return (char *)0;

	if ( !go_tail( list ) ) return (char *)0;

	return list_get_pointer( list );
}

void *list_first_pointer( LIST *list )
{
	return list_first( list );
}

void *list_get_first( LIST *list )
{
	return list_first( list );
}

void *list_first( LIST *list )
{
	if ( !go_head( list ) )
		return (void *)0;
	else
		return retrieve_item_ptr( list );
}

char *list_get_string( LIST *list )
{
	return retrieve_item_ptr( list );
}

boolean list_exists_string_beginning( LIST *list, char *string )
{
	char *item;

	if ( !list_rewind( list ) ) return 0;

	if ( !string ) return 0;

	do {
		item = list_get_pointer( list );

		if ( timlib_strncmp( string, item ) == 0 ) return 1;

	} while( list_next( list ) );

	return 0;

}

boolean list_item_exists( LIST *list, char *item, int (*match_fn)() )
{
	return item_exists( list, item, match_fn );
}

boolean list_exists( LIST *list, char *item, int (*match_fn)() )
{
	if ( !list ) return 0;
	if ( !item ) return 0;

	return item_exists( list, item, match_fn );
}

boolean list_search_string( LIST *list, char *string )
{
	if ( !list ) return 0;
	if ( !string ) return 0;

	return list_exists_string( string, list );
}

boolean list_string_exists( char *string, LIST *list )
{
	if ( !list ) return 0;
	if ( !string ) return 0;

	return item_exists( list, string, list_strcmp );
}

boolean list_exists_string( char *string, LIST *list )
{
	if ( !list ) return 0;
	if ( !string ) return 0;

	return item_exists( list, string, list_strcmp );
}

boolean list_exists_any_index_string( LIST *list, char *string )
{
	if ( !list ) return 0;
	if ( !string ) return 0;

	return item_exists( list, string, list_string_index_compare );
}

boolean list_is_subset_of( LIST *subset, LIST *set )
{
	return is_subset_of( subset, set );
}

LIST *list_intersect_string_list( LIST *list1, LIST *list2 )
{
	LIST *return_list = list_new();
	DICTIONARY *dictionary = dictionary_new();
	char *s;

	if ( !list_rewind( list1 ) ) return return_list;
	if ( !list_rewind( list2 ) ) return return_list;

	do {
		dictionary_add_string(	dictionary,
					list_get_string( list2 ),
					"" );
	} while( list_next( list2 ) );

	do {
		s = list_get_string( list1 );
		if ( dictionary_exists_key(
					dictionary,
					s ) )
		{
			list_append_pointer( return_list, s );
		}
	} while( list_next( list1 ) );

	/* Bug: dictionary_free( dictionary ); */
	return return_list;

}

boolean is_subset_of( LIST *subset, LIST *set )
{
	char *item;

	if ( !subset || !set ) return 0;

	list_save( subset );
	list_save( set );
	if ( list_reset( subset ) )
		do {
			item = list_get_pointer( subset );
			if ( !item_exists( set, item, list_strcmp ) )
			{
				list_restore( subset );
				list_restore( set );
				return 0;
			}
		} while ( next_item( subset ) );
	list_restore( subset );
	list_restore( set );
	return 1;
}

void list_delete_string( LIST *list, char *string )
{
	if ( item_exists( list, string, list_strcmp ) )
		delete_current( list );
}

void list_delete_from_list( LIST *set, LIST *delete_list )
{
	char *item;

	if ( list_reset( delete_list ) )
		do {
			item = list_get_pointer( delete_list );
			if ( item_exists( set, item, list_strcmp ) )
				delete_current( set );
		} while ( next_item( delete_list ) );
}

void list_display_lines( LIST *list )
{
	if ( !list_rewind( list ) ) return;

	do {
		printf( "%s\n", (char *)list_get_pointer( list ) );
	} while( list_next( list ) );
}

void list_html_display(	LIST *list )
{
	list_html_table_display(
		list,
		(char *)0 /* heading_string */ );
}

void list_html_table_display(	LIST *list,
				char *heading_string )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];

	if ( !list_rewind( list ) ) return;

	sprintf( sys_string,
		 "html_table.e '' '%s' '^'",
		 (heading_string)
			? heading_string
			: "" );

	output_pipe = popen( sys_string, "w" );

	do {
		fprintf( output_pipe,
			 "%s\n",
			 (char *)list_get( list ) );

	} while( list_next( list ) );

	pclose( output_pipe );
}

char *list_display( LIST *list )
{
	return list2comma_string( list );
}

char *list_string_display( LIST *list )
{
	return list2comma_string( list );
}

char *list_display_quoted( char *destination, LIST *list )
{
	return list_display_quoted_delimiter( 	destination, 
						list,	
						',' );
}

/* This will generate an in clause */
/* ------------------------------- */
char *list_display_quote_comma_delimited(
					char *destination, 
					LIST *list )
{
	char *dest_ptr = destination;
	char *ptr;
	boolean first_time = 1;

	if ( go_head( list ) )
		do {
			ptr = retrieve_item_ptr( list );
			if ( first_time )
			{
				first_time = 0;
				dest_ptr += sprintf( dest_ptr, "'%s'", ptr );
			}
			else
				dest_ptr += sprintf( dest_ptr, 
						    ",'%s'", 
						    ptr );
		} while( next_item( list ) );
	*dest_ptr = '\0';
	return destination;
}

char *list_display_double_quote_comma_delimited(
					char *destination, 
					LIST *list )
{
	char *dest_ptr = destination;
	char *ptr;
	boolean first_time = 1;

	if ( go_head( list ) )
		do {
			ptr = retrieve_item_ptr( list );
			if ( first_time )
			{
				first_time = 0;
				dest_ptr += sprintf( dest_ptr, "\"%s\"", ptr );
			}
			else
				dest_ptr += sprintf( dest_ptr, 
						    ",\"%s\"", 
						    ptr );
		} while( next_item( list ) );
	*dest_ptr = '\0';
	return destination;
}

char *list_display_quoted_delimiter( 	char *destination, 
					LIST *list,	
					char delimiter )
{
	char *dest_ptr = destination;
	char *ptr;
	boolean first_time = 1;

	*dest_ptr++ = '"';

	if ( go_head( list ) )
		do {
			ptr = retrieve_item_ptr( list );
			if ( first_time )
			{
				first_time = 0;
				dest_ptr += sprintf( dest_ptr, "%s", ptr );
			}
			else
				dest_ptr += sprintf( dest_ptr, 
						    "%c%s", 
						    delimiter,
						    ptr );
		} while( next_item( list ) );
	*dest_ptr++ = '"';
	*dest_ptr = '\0';
	return destination;
}

LIST *list_double_quotes_around_string_list( LIST *list )
{
	char buffer[ 1024 ];
	LIST *return_list = list_new();
	char *ptr;

	if ( go_head( list ) )
		do {
			ptr = retrieve_item_ptr( list );
			sprintf( buffer, "\"%s\"", ptr );
			list_append_string( return_list, buffer );
		} while( next_item( list ) );
	return return_list;
}

char *list_length_display(
			LIST *string_list,
			int how_many )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	char *string;
	boolean first_time = 1;

	if ( !list_rewind( string_list ) ) return "";

	*buf_ptr = '\0';

	do {
		string = list_get( string_list );

		if ( first_time )
		{
			first_time = 0;

			buf_ptr += sprintf( buf_ptr, 
					    "%s", 
					    string );
		}
		else
		{
			buf_ptr += sprintf( buf_ptr, 
					    "^%s", 
					    string );
		}

		if ( --how_many == 0 ) break;

	} while( list_next( string_list ) );

	return strdup( buffer );
}

char *list_display_limited(
			LIST *list,
			char delimiter,
			int length )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	char *item;
	boolean first_time = 1;

	if ( !list_rewind( list ) || !length ) return "";

	do {
		item = list_get( list );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c", delimiter );
		}

		ptr += sprintf(	ptr, 
				"%s", 
				item );

		if ( --length == 0 ) break;

	} while( next_item( list ) );

	return strdup( buffer );
}

char *list_display_delimited( LIST *list, char delimiter )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	char *ptr;
	boolean first_time = 1;

	if ( !list_rewind( list ) ) return "";

	*buf_ptr = '\0';

	do {
		ptr = retrieve_item_ptr( list );

		if ( first_time )
		{
			first_time = 0;

			buf_ptr += sprintf( buf_ptr, 
					    "%s", 
					    ptr );
		}
		else
		{
			buf_ptr += sprintf( buf_ptr, 
					    "%c%s", 
					    delimiter,
					    ptr );
		}
	} while( next_item( list ) );

	return strdup( buffer );
}

char *list_buffered_display(	char *destination,
				LIST *list, 
				char delimiter )
{
	char *buf_ptr = destination;
	char *ptr;
	boolean first_time = 1;

	if ( !go_head( list ) ) return "";

	*buf_ptr = '\0';

	do {
		ptr = retrieve_item_ptr( list );
		if ( first_time )
		{
			first_time = 0;
			buf_ptr += sprintf( buf_ptr, 
					    "%s", 
					    ptr );
		}
		else
			buf_ptr += sprintf( buf_ptr, 
					    "%c%s", 
					    delimiter,
					    ptr );
	} while( next_item( list ) );
	return destination;
}

char *list_display_delimited_plus_space(LIST *list, 
					char delimiter )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	char *ptr;
	boolean first_time = 1;

	if ( !go_head( list ) ) return "";

	*buf_ptr = '\0';

	do {
		ptr = retrieve_item_ptr( list );
		if ( first_time )
		{
			first_time = 0;
			buf_ptr += sprintf( buf_ptr, 
					    "%s", 
					    ptr );
		}
		else
			buf_ptr += sprintf( buf_ptr, 
					    "%c %s", 
					    delimiter,
					    ptr );
	} while( next_item( list ) );
	return strdup( buffer );
}

char *list_double_display(	LIST *double_list, 
				char delimiter )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	double *ptr;
	boolean first_time = 1;

	if ( !list_rewind( double_list ) ) return "";

	*buf_ptr = '\0';

	do {
		ptr = (double *)retrieve_item_ptr( double_list );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			buf_ptr += sprintf( buf_ptr, 
					    "%c", 
					    delimiter );
		}

		buf_ptr += sprintf(
				buf_ptr, 
				"%.2lf", 
				*ptr );

	} while( list_next( double_list ) );

	return strdup( buffer );

}

char *list_display_delimited_prefixed(	LIST *list, 
					char delimiter, 
					char *prefix )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	char *ptr;
	boolean first_time = 1;

	if ( !list_rewind( list ) ) return "";

	if ( !prefix ) prefix = "";

	*buf_ptr = '\0';

	do {
		ptr = retrieve_item_ptr( list );
		if ( first_time )
		{
			first_time = 0;
			buf_ptr += sprintf( buf_ptr, 
					    "%s%s", 
					    prefix,
					    ptr );
		}
		else
			buf_ptr += sprintf( buf_ptr, 
					    "%c%s%s", 
					    delimiter,
					    prefix,
					    ptr );
	} while( next_item( list ) );

	return strdup( buffer );

}

char *list2comma_string( LIST *list )
{
	return list_display_delimited( list, ',' );
}

LIST *list_subtract_string_list( LIST *big_list, LIST *subtract_this )
{
	return list_subtract( big_list, subtract_this );
}

LIST *subtract_list( LIST *big_list, LIST *subtract_this )
{
	return list_subtract( big_list, subtract_this );
}

LIST *list_subtract_list( LIST *big_list, LIST *subtract_this )
{
	return list_subtract( big_list, subtract_this );
}

LIST *list_subtract( LIST *big_list, LIST *subtract_this )
{
	char *item;
	LIST *return_list = create_list();

	if ( go_head( big_list ) )
		do {
			item = (char *)retrieve_item_ptr( big_list );
			if ( !item_exists( subtract_this, item, list_strcmp ) )
				list_append_pointer( 	return_list, 
							item );
		} while( next_item( big_list ) );
	return return_list;
}

/* index is one based */
/* ------------------ */
char *list_seek_index(	LIST *list,
			int index )
{
	if ( list_go_offset( list, index - 1 ) )
		return (char *)retrieve_item_ptr( list );
	else
		return (char *)0;
}

/* offset is zero based */
/* -------------------- */
char *list_seek_offset( LIST *list, int offset )
{
	return list_get_offset( list, offset );
}

/* offset is zero based */
/* -------------------- */
char *list_get_offset( LIST *list, int offset )
{
	if ( list_go_offset( list, offset ) )
		return (char *)retrieve_item_ptr( list );
	else
		return (char *)0;
}

/* offset is zero based. */
/* --------------------- */
boolean list_go_offset( LIST *list, int offset )
{
	int i = 0;

	if ( go_head( list ) )
		do {
			if ( i++ == offset ) return 1;
		} while ( next_item( list ) );
	return 0;
}

void list_append_string( LIST *list, char *string )
{
	append( list, string, strlen( string ) + 1 );
}

void list_add_string( LIST *list, char *string )
{
	append( list, string, strlen( string ) + 1 );
}

LIST *list_append_current_list(	LIST *destination_list,
				LIST *source_list )
{
	void *a;

	if ( !destination_list ) return destination_list;

	if ( list_past_end( source_list ) ) return destination_list;

	do {
		a = list_get_pointer( source_list );
		list_append_pointer( destination_list, a );
	} while( list_next( source_list ) );

	return destination_list;

}

LIST *list_append_list(	LIST *destination_list,
			LIST *source_list )
{
	return list_set_list( destination_list, source_list );
}

LIST *list_set_list(	LIST *destination_list,
			LIST *source_list )
{
	void *a;

	if ( !destination_list ) return destination_list;

	if ( list_reset( source_list ) )
	{
		do {
			a = list_get( source_list );
			list_set( destination_list, a );
		} while( list_next( source_list ) );
	}

	return destination_list;

}

void list_append( LIST *list, void *this_item, int num_bytes )
{
	append( list, this_item, num_bytes );
}

int list_length( LIST *list )
{
	return num_in_list( list );
}

LIST *pipe2list_append( LIST *source_list, char *pipe_string )
{
	list_load_from_pipe( source_list, pipe_string );
	return source_list;
}

LIST *pipe2list( char *pipe_string )
{
	LIST *list = create_list();
	list_load_from_pipe( list, pipe_string );
	return list;
}

void get_list_from_pipe( LIST *list, char *pipe_string )
{
	list_load_from_pipe( list, pipe_string );
}

void list_get_from_pipe( LIST *list, char *pipe_string )
{
	list_load_from_pipe( list, pipe_string );
}

LIST *list_fetch_pipe( char *system_string )
{
	return list_pipe_fetch( system_string );
}

LIST *list_pipe_fetch( char *system_string )
{
	char buffer[ 65536 ];
	FILE *p;
	LIST *list = list_new();

	p = popen( system_string, "r" );

	while( string_input( buffer, p, 65536 ) )
	{
		list_set( list, strdup( buffer ) );
	}

	pclose( p );
	return list;
}

void list_load_from_pipe( LIST *list, char *pipe_string )
{
	char buffer[ 65536 ];
	FILE *p;

	p = popen( pipe_string, "r" );

	while( timlib_get_line( buffer, p, 65536 ) )
	{
		append( list, buffer, strlen( buffer ) + 1 );
	}

	pclose( p );
}

int list_at_first( LIST *list )
{
	return at_head( list );
}

int list_at_head( LIST *list )
{
	return at_head( list );
}

int at_head( LIST *list )
{
        return (list->current == list->head->next);

}


int list_at_tail( LIST *list )
{
	return at_tail( list );
}

int at_tail( LIST *list )
{
        return (list->current == list->tail->previous);

}



int delete_item( LIST *list )
{
	return delete_current( list );
}

int delete_current( LIST *list )
/* --------------------------------------------------- */
/* This function deletes the item pointing to current. */
/* If the list is empty, it returns FALSE.             */
/* --------------------------------------------------- */
{
        struct LINKTYPE *save_ptr;
        if ( !list->num_in_list ) return 0;

        /* Free item space */
        /* --------------- */
        /* free (list->current->item); assume done outside */
        save_ptr = list->current;

	if ( list->current )
	{
		if ( list->current->previous )
		{
	        	list->current->previous->next = list->current->next;
		}
	
		if ( list->current->next )
		{
	        	list->current->next->previous =
				list->current->previous;
		}
	}

        list->num_in_list--;

	if ( save_ptr )
	{
        	/* Note: an empty list must point to the tail */
        	/* ------------------------------------------ */
		if ( !list->num_in_list )
        		list->current = save_ptr->next;
		else
        		list->current = save_ptr->previous;

        	free ( save_ptr );
	}

        return 1;

}

int list_count( LIST *list )
{
	return num_in_list( list );
}

int num_in_list( LIST *list )
{
	if ( !list )
		return 0;
	else
        	return list->num_in_list;
}

void *list_match_seek( LIST *list, char *string, int (*match_fn)() )
{
	if ( list_exists( list, string, match_fn ) )
		return list->current->item;
	else
		return (void *)0;
}

int item_exists( LIST *list, char *item, int (*match_fn)() )
/* ----------------------------------------------------------- */
/* This function return the offset if item exists in the list, */
/* otherwise it returns 0.	                               */
/* Also, if found the current pointer points there.            */
/* ----------------------------------------------------------- */
{
	int offset = 0;
        struct LINKTYPE *save_ptr;

        if ( !go_head( list ) ) return 0;

        save_ptr = list->current;
        while ( 1 )
	{
		offset++;
                if ((*match_fn) (list->current->item, item) == 0)
		{
                        return offset;
		}
                else
                if (!next_item(list))
		{
                        list->current = save_ptr;
                        return 0;
                }
	}
}

struct LINKTYPE *create_node()
/* --------------------------- */
/* Allocate a new node to link */
/* --------------------------- */
{
        return (struct LINKTYPE *)calloc( 1, sizeof( struct LINKTYPE ) );

}

void list_free_string_list( LIST *string_list )
{
	if ( list_rewind( string_list ) )
		do {
			free( list_get_string( string_list ) );
		} while( list_next( string_list ) );
	list_free_container( string_list );
}

void free_string_list( LIST *string_list )
{
	free_string_list( string_list );
}

void list_free( LIST *list )
{
	destroy_list( list );
}

int destroy_list( LIST *list )
/* ---------------------------------------------------- */
/* This function frees all item space in the nodes,     */
/* delete all nodes in the list, and frees up the list. */
/* ---------------------------------------------------- */
{
        if (go_head( list ) )
                while ( list->num_in_list )
                        delete_current( list );
        free( list );
	return 1;
}

LIST *new_list() { return create_list(); }
LIST *list_new_list() { return create_list(); }
LIST *list_new() { return create_list(); }

LIST *create_list( void )
/* ------------------------------------------- */
/* Create A New List with a list head and tail */
/* ------------------------------------------- */
{
        LIST *ret_list = (LIST *)calloc(1, sizeof( LIST ) );
        struct LINKTYPE *list_head, *list_tail;

        if (!ret_list)
                list_bye("Memory Allocation Error In create_list");

        list_head = (struct LINKTYPE *)calloc(1,sizeof(struct LINKTYPE));
        if (!list_head)
                list_bye("Memory Allocation Error In create_list");

        list_tail = (struct LINKTYPE *)calloc(1,sizeof(struct LINKTYPE));
        if (!list_tail)
                list_bye("Memory Allocation Error In create_list");

        ret_list->head = list_head;

        /* Note: an empty list must point to the tail */
        /* ------------------------------------------ */
        ret_list->tail = ret_list->current = list_tail;

        /* Join the lists together */
        /* ----------------------- */
        list_head->next = list_tail;
        list_head->previous = (struct LINKTYPE *)0;

        list_tail->next = (struct LINKTYPE *)0;
        list_tail->previous = list_head;

        ret_list->num_in_list = 0;

        return ret_list;

}



boolean list_reset( LIST *list )
{
	return go_head( list );
}

boolean list_rewind( LIST *list )
{
	return go_head( list );
}

boolean go_head( LIST *list )
{
        if ( !list || !list->num_in_list ) return 0;

	list->past_end = 0;
        list->current = list->head->next;

        return 1;

}

void list_insert_head( LIST *list, void *item, int num_bytes )
{
	add_item(	list,
			item,
			num_bytes,
			ADD_HEAD );
}

void list_insert_current( LIST *list, void *item )
{
	list_set_current( list, item );
}

void list_set_current_pointer(	LIST *list, void *item )
{
	list_set_current( list, item );
}

void list_set_position_string(	LIST *list,
				char *string,
				int position )
{
	if ( !list_rewind( list ) ) return;

	do {
		if ( !position )
		{
			add_item(	list,
					string,
					strlen( string ) + 1,
					ADD_CURRENT );
			return;
		}
		position--;
	} while( list_next( list ) );
}

void list_set_current( LIST *list, void *item )
{
	list->current->item = item;
}

void list_set_string( LIST *list, char *s )
{
	list_set_current_string( list, s );
}

void list_set_current_string( LIST *list, char *s )
{
        list->current->item = strdup( s );
        list->current->num_bytes = strlen( s ) + 1;
}

int list_go_head( LIST *list )
{
	return go_head( list );
}

boolean list_go_last( LIST *list )
{
	return (boolean)go_tail( list );
}

/* ----------------------------------------------- */
/* Move the current pointer to the end of the list */
/* ----------------------------------------------- */
boolean list_tail( LIST *list )
{
        if ( !list || !list->num_in_list)
                return 0;

        list->current = list->tail->previous;
        return 1;
}

boolean list_go_tail( LIST *list )
{
	return list_tail( list );
}

boolean go_tail( LIST *list )
{
	return list_tail( list );
}

struct LINKTYPE *get_current_record( LIST *list )
{
	return list->current;

}

void go_record( LIST *list, struct LINKTYPE *this_record )
{
	list->current = this_record;

}


boolean list_still_more( LIST *list )
{
	if ( list && list->num_in_list )
		return !list->past_end;
	else
		return 0;
}

boolean list_past_end( LIST *list )
{
	if ( list && list->num_in_list )
		return list->past_end;
	else
		return 1;
}

int list_at_end( LIST *list )
{
	return at_end( list );
}

int list_at_start( LIST *list )
{
        if ( list->current == list->head->next )
                return 1;
	else
                return 0;
}

int at_end( LIST *list )
{
        if (list->current->next == list->tail)
                return 1;
	else
                return 0;
}

boolean list_prior( LIST *list )
{
	return (boolean)previous_item( list );
}

boolean list_previous( LIST *list )
{
	return (boolean)previous_item( list );
}

int list_next( LIST *list )
{
	return next_item( list );
}

int next_item( LIST *list )
/* ----------------------------------------- */
/* Set the current pointer to the next link. */
/* If at the end of the list, return FALSE.  */
/* ----------------------------------------- */
{
        if ( !list
	||   !list->num_in_list
        ||    list->current->next == list->tail )
	{
		if ( list ) list->past_end = 1;
                return 0;
	}
        else
        {
                list->current = list->current->next;
                return 1;
        }

}


int previous_item( LIST *list )
/* ---------------------------------------------- */
/* Set the current pointer to the previous link.  */
/* If at the beginning of the list, return FALSE. */
/* ---------------------------------------------- */
{
	list->past_end = 0;

        if (list->current->previous == list->head)
	{
                return 0;
	}
        else
        {
                list->current = list->current->previous;
                return 1;
        }

}


void *list_get_current_pointer( LIST *list )
{
	return retrieve_item_ptr( list );
}

void *list_get_pointer( LIST *list )
{
	return retrieve_item_ptr( list );
}

void *list_pointer( LIST *list )
{
	return retrieve_item_ptr( list );
}

void *list_string( LIST *list )
{
	return retrieve_item_ptr( list );
}

void *list_get( LIST *list )
{
        if (list->num_in_list)
                return list->current->item;
        else
                /* List is empty */
                /* ------------- */
                return (char *)0;
}

char *retrieve_item_ptr( LIST *list )
{
	return list_get( list );
}


int retrieve_item( char *ret_item, LIST *list )
/* ------------------------------------------------------ */
/* Retrieve the item from the current pointer in the list */
/* ------------------------------------------------------ */
{
        if (list->num_in_list
	&&  list->current->num_bytes != -1){
                moveitem(       ret_item,
				list->current->item,
                                list->current->num_bytes);
                return 1;
        }
        else
                /* List is empty */
                /* ------------- */
                return 0;

}

boolean list_add_string_in_order(	LIST *list,
					char *string )
{
	return list_add_pointer_in_order(
			list, 
			string,
			strcasecmp );

}

boolean list_add_pointer_in_order(	LIST *list, 
					void *this_item, 
					int (*match_fn)() )
{
        struct LINKTYPE *newlink = create_node();

        if (!newlink)
                list_bye( "create_node failed in list_add_pointer_in_order()" );

        newlink->item = this_item;
        newlink->num_bytes = -1;

        /* If list is empty then just add the item */
        /* --------------------------------------- */
	if (!list_rewind( list ) )
	{
		list_append_pointer( list, this_item );
		return 1;
	}
	else
	/* Find where the item needs to go */
	/* ------------------------------- */
	do {
		if ((*match_fn) (list->current->item, this_item) >= 0 )
		{
			/* Add someplace in the middle */
			/* --------------------------- */
			list->current->previous->next = newlink;
			newlink->previous = list->current->previous;
			newlink->next = list->current;
			list->current->previous = newlink;
			list->num_in_list++;
			return 1;
		}
	} while ( list_next( list ) );

        /* If at the end of the list then just add to the tail */
        /* -------------------------------------------------- */
        list_append_pointer( list, this_item );

        return 1;

}

int list_add_in_order(	LIST *list, 
			void *this_item, 
			int num_bytes, 
			int (*match_fn)() )
{
	return add_in_order( list, this_item, num_bytes, match_fn );
}


int add_in_order( 	LIST *list, 
			void *this_item, 
			int num_bytes,
			int (*match_fn)() )
/* --------------------------------------------------------- */
/* Add a new link containing this item to the list in order  */
/* The current pointer is uneffected.                        */
/* --------------------------------------------------------- */
{
        /* Create the new link */
        /* ------------------- */
        struct LINKTYPE *create_node(),*newlink = create_node();

        if (!newlink)
                return 0;

        newlink->item = (char *)malloc(num_bytes);
        if (!newlink->item)
                return 0;


        moveitem(newlink->item,this_item,num_bytes);
        newlink->num_bytes = num_bytes;

        /* If list is empty then just add the item */
        /* --------------------------------------- */
        if (!go_head(list)){
                add_item(list,this_item,num_bytes,ADD_HEAD);
                return 1;
        }
        else
                /* Find where the item needs to go */
                /* ------------------------------- */
                do {
                        if ((*match_fn) ( list->current->item, 
					 this_item) >= 0 )
			{
                                /* Add someplace in the middle */
                                /* --------------------------- */
                                list->current->previous->next = newlink;
                                newlink->previous = list->current->previous;
                                newlink->next = list->current;
                                list->current->previous = newlink;
                                list->num_in_list++;
                                return 1;
                        }
                } while (next_item(list));

        /* If at the end of the list then just add to the tail */
        /* -------------------------------------------------- */
        add_item(list,this_item,num_bytes,ADD_TAIL);
        return 1;
}

LIST *list_unique_list(
			LIST *destination_list,
			LIST *source_list )
{
	return list_append_unique_string_list(
			destination_list,
			source_list );
}

LIST *list_append_unique_string_list(
			LIST *destination_list,
			LIST *source_list )
{
	char *item;

	if ( list_rewind( source_list ) )
	{
		do {
			item = list_get_pointer( source_list );
			list_append_unique_string( destination_list, item );
		} while( list_next( source_list ) );
	}
	return destination_list;
}

void list_append_unique_string(
			LIST *list,
			char *item )
{
	list_set_unique( list, item );
}

void list_unique_set(
			LIST *list,
			char *this_item )
{
	list_set_unique( list, this_item );
}

void list_set_unique(
			LIST *list,
			char *this_item )
{
	if ( !item_exists( list, this_item, strcmp ) )
	{
		list_set( list, this_item );
	}
}

char *list_append_unique(	LIST *list,
				char *this_item,
				int num_bytes,
				int (*match_fn)() )
{
	if ( item_exists( list, this_item, match_fn ) )
		return this_item;
	else
		return add_item( list, this_item, num_bytes, ADD_TAIL );

}

char *append( LIST *list, char *this_item, int num_bytes )
{
	return add_item( list, this_item, num_bytes, ADD_TAIL );

}

char *add_item(	LIST *list,
		void *this_item,
		int num_bytes,
		int head_or_tail_or_current )
/* ------------------------------------------------- */
/* Add a new link containing this item to the list   */
/* The current pointer is uneffected.                */
/* ------------------------------------------------- */
{
        struct LINKTYPE *create_node(),*newlink;

	if ( !list ) return (char *)0;

        /* Create the new link */
        /* ------------------- */
        newlink = create_node();

        if (!newlink)
                list_bye( "create_node failed in add_item()" );;

        newlink->item = (char *)malloc(num_bytes);
        if (!newlink->item)
                list_bye( "malloc failed in add_item()" );

        moveitem(newlink->item,this_item,num_bytes);
        newlink->num_bytes = num_bytes;

        if (head_or_tail_or_current == ADD_HEAD)
        {
                newlink->next = list->head->next;
                newlink->previous = list->head;
                newlink->next->previous = newlink;
                list->head->next = newlink;
        }
        else
        if (head_or_tail_or_current == ADD_TAIL)
        {
                newlink->previous = list->tail->previous;
                newlink->next = list->tail;
                newlink->previous->next = newlink;
                list->tail->previous = newlink;
        }
        else
        if (head_or_tail_or_current == ADD_CURRENT)
        {
                newlink->previous = list->current->previous;
                newlink->next = list->current;
                newlink->previous->next = newlink;
                list->current->previous = newlink;
        }
        else
                list_bye ("Invalid Parameter In add_item");

        list->num_in_list++;
	list->current = newlink;

	/* Return the memory allocated */
	/* --------------------------- */
        return newlink->item;

}


void moveitem( char *to, char *from, int num_bytes )
{
        int x;

        for(x = 0;x < num_bytes;x++) *to++ = *from++;
}



void list_bye( char *string )
{
        fprintf(stderr,"list_bye: %s\n",string);
        exit(1);
}

/* Sample: "station","grade","value_1" */
/* ----------------------------------- */
LIST *list_trim_indices( LIST *string_list )
{
	LIST *new_list = list_new();
	char *ptr, buffer[ 1024 ];

	if ( list_rewind( string_list ) )
	{
		do {
			ptr = list_get_pointer( string_list );
			list_append_string( new_list,
					    trim_index( buffer,
							ptr ) );
		} while( list_next( string_list ) );
	}
	return new_list;
}

LIST *list_unique( LIST *string_list )
{
	LIST *new_list = list_new();
	char *ptr;

	if ( list_reset( string_list ) )
		do {
			ptr = list_get_pointer( string_list );
			list_append_unique( 
				new_list,
				ptr,
				strlen( ptr ) + 1,
				list_strcmp );
	
		} while( next_item( string_list ) );
	return new_list;
}

LIST *list_delimiter_string_to_integer_list(
				char *list_string,
				char delimiter )
{
	LIST *list = list_new();
	char buffer[ 8192 ];
	int *integer_ptr;
	int i;

	if ( list_string )
	{
		for ( i = 0; piece( buffer, delimiter, list_string, i ); i++ )
		{
			if ( ! ( integer_ptr = (int *)
					calloc( 1, sizeof( int ) ) ) )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate integer memory.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			list_append_pointer( list, integer_ptr );
		}
	}
	return list;
}

LIST *list_string_extract( char *list_string, char delimiter )
{
	return list_string_list( list_string, delimiter );
}

LIST *list_delimiter_string_to_list( char *list_string, char delimiter )
{
	return list_string_list( list_string, delimiter );
}

LIST *list_string_to_list( char *list_string, char delimiter )
{
	return list_string_list( list_string, delimiter );
}

LIST *list_string2list( char *list_string, char delimiter )
{
	return list_string_list( list_string, delimiter );
}

LIST *string2list( char *list_string, char delimiter )
{
	return list_string_list( list_string, delimiter );
}

LIST *list_quote_comma_string2list( char *list_string )
{
	LIST *list = list_new();
	char buffer[ 8192 ];
	int i;

	if ( list_string )
	{
		for (	i = 0;
			piece_quoted( buffer, ',', list_string, i, '"' );
			i++ )
		{
			list_append_pointer( list, strdup( buffer ) );
		}
	}
	return list;
}

/* Free all memory with list_free_string_list() */
/* -------------------------------------------- */
LIST *list_string_list(
			char *list_string,
			char delimiter )
{
	LIST *list = list_new();
	char buffer[ 8192 ];
	int i;

	if ( list_string && *list_string )
	{
		for ( i = 0; piece( buffer, delimiter, list_string, i); i++ )
		{
			list_set( list, strdup( buffer ) );
		}
	}
	return list;
}

LIST *list_duplicate( LIST *list )
{
	return string_list_duplicate( list );
}

LIST *list_duplicate_string_list( LIST *list )
{
	return string_list_duplicate( list );
}

LIST *string_list_duplicate( LIST *list )
{
	LIST *return_list = list_new();
	char *string;

	if( list_reset( list ) )
	{
		do {
			string = list_get_string( list );
			list_append_string( return_list, string );
		} while( list_next( list ) );
	}
	return return_list;
}

char *list_get_last_string( LIST *list )
{
	if ( !list_length( list ) ) return (char *)0;
	go_tail( list );
	return list_get_string( list );
}

void list_replace_last_string( 	LIST *list, char *string )
{
	if ( list_length( list ) )
	{
		go_tail( list );
		delete_current( list );
		list_append_string( list, string );
	}
}

void *list_get_first_item( LIST *list )
{
	if ( !list_rewind( list ) ) return "";
	return list_get_pointer( list );
}

int list_get_max_string_width( LIST *list )
{
	char *item;
	int max = 0;
	int str_len;

	if ( list_rewind( list ) )
		do {
			item = list_get_pointer( list );
			str_len = strlen( item );
			if ( str_len > max ) max = str_len;
		} while( list_next( list ) );
	return max;
}

LIST *list_copy_string_list( LIST *source )
{
	LIST *return_list = list_new();
	char *data;

	if ( list_rewind( source ) )
		do {
			data = list_get_string( source );
			list_set( return_list, data );
		} while( list_next( source ) );
	return return_list;
}

LIST *list_copy( LIST *source )
{
	LIST *return_list = list_new_list();
	char *data;

	if ( !list_rewind( source ) ) return return_list;

	do {
		data = list_get_pointer( source );
		list_append_pointer( return_list, data );
	} while( list_next( source ) );
	return return_list;
}


void list_push( LIST *list )
{
	return list_mark_current( list );
}

void list_pop( LIST *list )
{
	list_restore_current( list );
}

void list_push_current( LIST *list )
{
	list_mark_current( list );
}

void list_pop_current( LIST *list )
{
	list_restore_current( list );
}

void list_mark_current( LIST *list )
{
	if ( list ) list->mark = list->current;
}

void list_restore_current( LIST *list )
{
	if ( list ) list->current = list->mark;
}

void list_save( LIST *list )
{
	list_mark_current( list );
}

void list_restore( LIST *list )
{
	list_restore_current( list );
}

void list_double_forward( LIST *double_list, double here )
{
	double *double_pointer;

	do {
		double_pointer = (double *)list_get_pointer( double_list );
		if ( *double_pointer == here ) return;
	} while( list_next( double_list ) );
}

char *list2string_delimited( LIST *list, char delimiter )
{
	return list_display_delimited( list, delimiter );
}

char *list_to_string( LIST *list, char delimiter )
{
	return list_display_delimited( list, delimiter );
}

void list_toupper( LIST *list )
{
	if ( list_rewind( list ) )
	{
		do {
			up_string( list_get_string( list ) );
		} while( list_next( list ) );
	}
}


char *list_get_first_string( LIST *list )
{
	if ( list_rewind( list ) )
		return list_get_string( list );
	else
		return (char *)0;
}

int list_string_index_compare( char *s1, char *s2 )
{
	char s1_buffer[ 128 ];
	char s2_buffer[ 128 ];

	trim_index( s1_buffer, s1 );
	trim_index( s2_buffer, s2 );

	return list_strcmp( s1_buffer, s2_buffer );
}

int list_strcmp( char *s1, char *s2 )
{
	while( *s1 )
	{
		if ( toupper( *s1 ) < toupper( *s2 ) )
			return -1;
		else
		if ( toupper( *s1 ) > toupper( *s2 ) )
			return 1;
		s1++;
		s2++;
	}
	if ( *s2 )
		return -1;
	else
		return 0;
}

char *list_display_quoted_delimited( 	char *destination, 
					LIST *list,
					char delimiter )
{
	return list_display_quoted_delimiter(
					destination,
					list,
					delimiter );
}

LIST *list_replicate_string_list( char *string, int how_many )
{
	LIST *list = list_new();

	while( how_many-- ) list_append_string( list, string );
	return list;
}

LIST *string_array2string_list( char **string_array,
				int max_array_elements )
{
	LIST *list = list_new();
	int i;

	for ( i = 0; i < max_array_elements; i++ )
		if ( string_array[ i ] )
			list_append_pointer( list, string_array[ i ] );
	return list;
}

void list_remove_string(	LIST *string_list,
				char *remove_string )
{
	list_subtract_string( string_list, remove_string );
}

void list_replace_string( LIST *list, char *old, char *new )
{
	char *data;

	if ( list_rewind( list ) )
	{
		do {
			data = list_get_pointer( list );
			if ( strcmp( data, old ) == 0 )
			{
                		list->current->item = new;
			}
		} while( list_next( list ) );
	}
}

LIST *list_merge_string_list(	LIST *list1,
				LIST *list2,
				char delimiter )
{
	char *item1, *item2;
	char merged_items[ 4096 ];
	LIST *return_list = list_new();

	if ( list_rewind( list1 ) && list_rewind( list2 ) )
	{
		do {
			item1 = list_get_pointer( list1 );
			item2 = list_get_pointer( list2 );

			sprintf(merged_items,
				"%s%c%s",
				item1, delimiter, item2 );

			list_append_pointer(	return_list,
						strdup( merged_items ) );

			if ( !list_next( list2 ) ) break;
		} while( list_next( list1 ) );
	}
	return return_list;
}

boolean list_equivalent_string_list(
				LIST *list1,
				LIST *list2 )
{
	char *item;

	if (  !list_length( list1 )
	||   ( list_length( list1 ) != list_length( list2 ) ) )
	{
		return 0;
	}

	list_save( list1 );
	list_save( list2 );
	list_reset( list1 );
	do {
		item = list_get_pointer( list1 );

		if ( !list_item_exists( list2, item, list_strcmp ) )
		{
			list_restore( list1 );
			list_restore( list2 );
			return 0;
		}
	} while ( next_item( list1 ) );
	list_restore( list1 );
	list_restore( list2 );
	return 1;
}

boolean list_equals_string_list(LIST *list1,
				LIST *list2 )
{
	char *item1;
	char *item2;

	if (  !list_length( list1 )
	||   ( list_length( list1 ) != list_length( list2 ) ) )
	{
		return 0;
	}

	list_rewind( list1 );
	list_rewind( list2 );

	do {
		item1 = list_get_pointer( list1 );
		item2 = list_get_pointer( list2 );

		if ( timlib_strcmp( item1, item2 ) != 0 ) return 0;

		list_next( list2 );
	} while ( list_next( list1 ) );

	return 1;

}

LIST *list_get_position_list(	LIST *list,
				LIST *subset_list )
{
	char position_string[ 16 ];
	char *item;
	LIST *position_list = list_new_list();
	int position;

	if ( list_rewind( subset_list ) )
	{
		do {
			item = list_get_pointer( subset_list );
			if ( ( position = list_exists_string( item, list ) ) )
			{
				sprintf( position_string, "%d", position - 1 );
				list_append_pointer(
					position_list,
					strdup( position_string ) );
			}
		} while( list_next( subset_list ) );
	}
	return position_list;
}

void list_format_initial_capital( LIST *list )
{
	char *ptr;

	if ( list_rewind( list ) )
	{
		do {
			ptr = list_get_pointer( list );
			format_initial_capital( ptr, ptr );
		} while( list_next( list ) );
	}
}

boolean list_string_list_same(	LIST *list1,
				LIST *list2 )
{
	return list_string_list_match( list1, list2 );
}

boolean list_string_list_match( LIST *list1,
				LIST *list2 )
{
	char *item1, *item2;

	if ( !list1 || !list2 ) return 0;
	if ( list_length( list1 ) != list_length( list2 ) ) return 0;
	list_rewind( list1 );
	list_rewind( list2 );
	do {
		item1 = list_get_pointer( list1 );
		item2 = list_get_pointer( list2 );
		if ( strcmp( item1, item2 ) != 0 ) return 0;
		list_next( list2 );
	} while( list_next( list1 ) );

	return 1;

}

boolean list_string_list_all_populated( LIST *list )
{
	char *item;

	if ( !list_rewind( list ) ) return 0;
	do {
		item = list_get_pointer( list );
		if ( !*item ) return 0;
	} while( list_next( list ) );
	return 1;
}

boolean list_string_list_all_empty( LIST *list )
{
	char *item;

	if ( !list_rewind( list ) ) return 1;
	do {
		item = list_get_pointer( list );
		if ( *item ) return 0;
	} while( list_next( list ) );
	return 1;
}

LIST *list_rotate( LIST *list )
{
	void *last_item;

	if ( !list_length( list ) ) return list;
	list_go_tail( list );
	last_item = list_get_pointer( list );
	list_delete_current( list );
	list_go_head( list );
	list_set_current( list, last_item );
	return list;
}

LIST *list_purge_duplicates( LIST *string_list )
{
	LIST *return_list;
	char *string;

	return_list = list_new_list();

	if ( list_rewind( string_list ) )
	{
		do {
			string = list_get_pointer( string_list );
			if ( !list_exists_string(
					string,
					return_list ) )
			{
				list_append_pointer( return_list, string );
			}
		} while( list_next( string_list ) );
	}
	return return_list;
}

LIST *list_tail_list(		LIST *list,
				int include_tail_count )
{
	LIST *return_list;
	char *data;
	int count = 0;

	if ( list_length( list ) < include_tail_count ) return (LIST *)0;

	return_list = list_new();
	list_go_tail( list );

	while ( 1 )
	{
		data = list_get( list );
		list_prepend_pointer( return_list, data );

		if ( ++count == include_tail_count ) break;
		list_previous( list );
	}

	return return_list;

}

LIST *list_copy_count(		LIST *list,
				int count )
{
	LIST *return_list;
	void *data;
	int i;

	/* if ( count > list_length( list ) ) return (LIST *)0; */

	return_list = list_new();
	list_rewind( list );
	i = 0;

	do {
		data = list_get( list );
		list_append_pointer( return_list, data );
		if ( ++i == count ) break;

	} while( list_next( list ) );

	return return_list;

}

LIST *list_delimited_string_to_list(
			char *delimited_string )
{
	char delimiter;

	if ( ! ( delimiter = timlib_get_delimiter( delimited_string ) ) )
	{
		/* Could be anything. */
		/* ------------------ */
		delimiter = ';';
	}

	return list_delimiter_string_to_list( delimited_string, delimiter );

}

LIST *list_delimiter_list_piece_list(
				LIST *list,
				char delimiter,
				int piece_offset )
{
	LIST *return_list;
	char piece_buffer[ 8192 ];
	char *item;

	if ( !list_rewind( list ) ) return (LIST *)0;

	return_list = list_new();

	do {
		item = list_get_pointer( list );

		if ( piece( piece_buffer, delimiter, item, piece_offset ) )
		{
			list_append_pointer(
				return_list,
				strdup( piece_buffer ) );
		}
		else
		{
			list_append_pointer(
				return_list,
				strdup( "" ) );
		}
	} while( list_next( list ) );

	return return_list;

}

char *list_double_list_display(	char *destination,
				LIST *double_list )
{
	char *anchor = destination;
	double *d_ptr;

	if ( !list_rewind( double_list ) ) return "";

	do {
		d_ptr = (double *)list_get_pointer( double_list );

		if ( anchor != destination )
		{
			destination +=
				sprintf( destination,
					 ", " );
		}

		destination +=
			sprintf( destination,
				 "%.2lf",
				 *d_ptr );

	} while( list_next( double_list ) );

	return anchor;

}

LIST *list_string_to_double_list(
				LIST *string_list )
{
	LIST *double_list;
	double *d_ptr;
	char *item;

	if ( !list_rewind( string_list ) ) return (LIST *)0;

	double_list = list_new();

	do {
		item = list_get_pointer( string_list );

		if ( ! ( d_ptr = (double *)
				calloc( 1, sizeof( double ) ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate double memory.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		*d_ptr = atof( item );
		list_append_pointer( double_list, d_ptr );

	} while( list_next( string_list ) );

	return double_list;

}

/* -------------------------------------------- */
/* Returns zero-based index of matching double. */
/* Returns -1 if no match.			*/
/* -------------------------------------------- */
int list_double_list_match(	LIST *double_list,
				double match )
{
	double *d_ptr;
	int index = 0;

	if ( !list_rewind( double_list ) ) return -1;

	do {
		d_ptr = (double *)list_get_pointer( double_list );

		if ( timlib_double_virtually_same( *d_ptr, match ) )
		{
			return index;
		}

		index++;

	} while( list_next( double_list ) );

	return -1;

}

LIST *list_cycle_right( LIST *list )
{
	void *data;
	int length = list_length( list );

	if ( length < 2 ) return list;

	list_go_tail( list );
	data = list_data( list );
	list_delete_current( list );
	list_add_head( list, data );

	return list;

}

char *list_integer_display(	LIST *integer_list,
				char delimiter  )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	char *ptr;
	boolean first_time = 1;

	if ( !list_rewind( integer_list ) ) return "";

	*buf_ptr = '\0';

	do {
		ptr = list_get_pointer( integer_list );

		if ( first_time )
		{
			first_time = 0;

			buf_ptr += sprintf( buf_ptr, 
					    "%d", 
					    *ptr );
		}
		else
			buf_ptr += sprintf( buf_ptr, 
					    "%c%d", 
					    delimiter,
					    *ptr );
	} while( next_item( integer_list ) );

	return strdup( buffer );
}

LIST *list_string_new( char *string )
{
	LIST *l = list_new();

	list_set( l, string );
	return l;
}

/* Returns -1 if not found */
/* ----------------------- */
int list_seek(		char *item,
			LIST *list )
{
	char *a;
	int offset;

	if ( !list_rewind( list ) ) return -1;
	if ( !item ) return -1;

	offset = 0;

	do {
		a = list_get( list );

		if ( string_strcmp( a, item ) == 0 )
			return offset;

		offset++;
	} while ( list_next( list ) );

	return -1;
}

