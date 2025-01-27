#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "loadlang.h"

char **global_argv;

main( argc, argv )
int argc;
char **argv;
{
	if ( argc < 2 )
	{
		fprintf(stderr, 
			"Usage: %s file.rules [argv 1, ...]\n", 
			argv[ 0 ] );
		exit( 1 );
	}

	global_argv = argv;
	set_everything_up();

	load_language_file_array( argv[ 0 ], argv[ 1 ] );

	program_counter = 0;
	for_each_command();

	if ( input_file && input_file != stdin ) fclose( input_file );

	if ( output_file && output_file != stdout ) fclose( output_file );

	if ( exception_file )
	{
		if ( end_of_file_marker )
			fprintf( exception_file, "%c", end_of_file_marker );

		fclose( exception_file );
	}

	return 0;
}


output_array()
{
	int i;

	for( i = 0; language_file_array[ i ]; i++ )
		fprintf( stderr, "(%s) next: %d  eof: %d\n",
			language_file_array[ i ]->line,
			language_file_array[ i ]->next_instruction,
			language_file_array[ i ]->eof_instruction);
}

for_each_command()
{
	int index;
	char temp_buffer[ 1024 ];
	char argument_line[ 1024 ];

	/* Break in middle */
	/* --------------- */
	while( 1 )
	{
		if ( debug_mode )
		{
			fprintf( error_file,
				 "loadlang: %s;\n", 
				 language_file_array[ program_counter ]->line );
		}

		/* Check if "exit" */
		/* --------------- */
		if ( strncmp( language_file_array[ program_counter ]->line,
			      "exit",
			      strlen( "exit" ) ) == 0 )
		{
			return 1;
		}

		/* Ignore blank lines and comments */
		/* ------------------------------- */
		if ( *language_file_array[ program_counter ]->line
		&&   *language_file_array[ program_counter ]->line != '#' )
		{
			/* Search table for function */
			/* ------------------------- */
			index = search_fn_array(
				language_file_array[ program_counter ]->line );
	
			/* If not in table */
			/* --------------- */
			if ( index == -1 )
			{
				fprintf( error_file, 
				"Invalid command: (%s)\n",
				language_file_array[ program_counter ]->line );

				exit( 1 );
			}
	
			/* Isolate the command argument line */
			/* --------------------------------- */
			strcpy( temp_buffer, 
				language_file_array[ program_counter ]->line );
	
			strcpy( argument_line, 
				trim( temp_buffer +
				      strlen( fn_array[ index ].fn_name ) ) );
	
			/* Execute the function and pass the argument line */
			/* ----------------------------------------------- */
			fn_array[ index ].fn( argument_line );
		}

		/* If for_each_row() got EOF */
		/* ------------------------- */
		if ( eof_indicator )
		{
			program_counter = 
			language_file_array[ program_counter ]->eof_instruction;

			eof_indicator = 0;
		}
		else
		{
			program_counter = 
			language_file_array[ program_counter]->next_instruction;
		}
	}
}


search_fn_array( command )
char *command;
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

}

set_everything_up()
{
	table_list = list_init();
	variable_table = table_init();

	table_set_column( variable_table, "variable" );
	table_set_column( variable_table, "data" );
	error_file = stderr;

	return 1;
}



load_language_file_array( program_name, filename )
char *program_name;
char *filename;
{
	char input_buffer[ 1024 ];
	char *ptr;
	int c;
	int line_number = 0;
	int end_of_file = 0;
	FILE *file_to_read_from;

	file_to_read_from = fopen( filename, "r" );
	if ( !file_to_read_from )
	{
		fprintf( error_file, "%s: file open error: %s\n",
			 program_name, filename );
		exit( 1 );
	}

	while( !end_of_file )
	{
		skip_white_spaces( file_to_read_from );

		if ( line_number == MAX_LINES - 1 )
		{
			fprintf( error_file, 
			"load_language_file_array: max lines exceeded: %d\n", 
				 MAX_LINES - 1 );

			exit( 1 );
		}

		/* Get single line */
		/* --------------- */
		ptr = input_buffer;
		while( 1 )
		{
			c = getc( file_to_read_from );

			/* Check if all done */
			/* ----------------- */
			if ( c == EOF )
			{
				end_of_file = 1;
				break;
			}
			else
			/* -------------------- */
			/* Check if end of line */
			/* -------------------- */
			if ( c == ';' )
			{
				*ptr = '\0';

				store_the_line_of_code( line_number++, 
							input_buffer );

				/* Get next line */
				/* ------------- */
				break;
			}
			else
				/* ------------------- */
				/* Make the assignment */
				/* ------------------- */
				*ptr++ = c;
		}
		
	} /* for each line */

	/* Have the last statement be the "exit" command */
	/* --------------------------------------------- */
	store_the_line_of_code( line_number++, "exit" );

	/* Terminate the array */
	/* ------------------- */
	language_file_array[ line_number ] = (LANGUAGE_LINE *)0;

	fclose( file_to_read_from );

	set_all_instruction_pointers();

	return 1;
}


set_all_instruction_pointers()
{
	int i;
	static int hold_for_each_row_index;

	for (	i = 0;
		language_file_array[ i ];
		i++ )
	{

		if ( strncmp( language_file_array[ i ]->line,
		      	      "for_each_row",
		      	      strlen( "for_each_row" ) ) == 0 )
		{
			language_file_array[ i ]->next_instruction = 
				i + 1;

			/* Search for "end_for" and add 1 */
			/* ------------------------------ */
			language_file_array[ i ]->eof_instruction = 
				search_for_end_for( i ) + 1;

			if ( language_file_array[ i ]->eof_instruction == -1 )
			{
				fprintf( stderr, 
			"ERROR: \"for_each_row;\" must have \"end_for;\"\n" );
				exit( 1 );
			}

			/* Set the hold_for_each_row_index */
			/* ------------------------------- */
			hold_for_each_row_index = i;
		}
		else
		if ( strncmp( language_file_array[ i ]->line,
		      	      "end_for",
		      	      strlen( "end_for" ) ) == 0 )
		{
			language_file_array[ i ]->next_instruction = 
				hold_for_each_row_index;

			language_file_array[ i ]->eof_instruction = 
				hold_for_each_row_index;
		}
		else
		/* ------------------ */
		/* Normal instruction */
		/* ------------------ */
		{
			language_file_array[ i ]->next_instruction = 
				i + 1;
			language_file_array[ i ]->eof_instruction = 
				i + 1;
		}
	}

	return 1;
}


search_for_end_for( here )
int here;
{
	for (	;
		language_file_array[ here ];
		here++ )
	{
		if ( strncmp( language_file_array[ here ]->line,
		      	      "end_for",
		      	      strlen( "end_for" ) ) == 0 )
		{
			return here;
		}
	}

	return -1;
}


skip_white_spaces( f )
FILE *f;
{
	int c;

	while( ( c = getc( f ) ) != EOF)
	{
		if ( !isspace( c ) ) break;
	}

	/* Went too far; place character back */
	/* ---------------------------------- */
	ungetc( c, f );
}


store_the_line_of_code( line_number, input_buffer )
int line_number;
char *input_buffer;
{
	LANGUAGE_LINE *language_line_ptr;
	int c;

	language_line_ptr = (LANGUAGE_LINE *)
				calloc( 1, sizeof( LANGUAGE_LINE ) );
	if ( !language_line_ptr )
	{
		fprintf( error_file, 
			"store_the_line_of_code: Memory allocation error.\n" );
		exit( 1 );
	}

	language_line_ptr->line = strdup( input_buffer );

	if ( !language_line_ptr->line )
	{
		fprintf( error_file, 
			 "store_the_line_of_code: strdup failed.\n" );
		exit( 1 );
	}

	language_file_array[ line_number ] = language_line_ptr;

	return 1;
}


int set_table_key( table_key )
char *table_key;
{
	LANGUAGE_TABLE l;

	current_table_key = strdup( table_key );
	l.table_key = strdup( table_key );
	l.table_load = table_load_init();

	add_item( table_list, &l, sizeof( LANGUAGE_TABLE ), ADD_TAIL );
	go_tail( table_list );
	current_language_table_ptr = (LANGUAGE_TABLE *)
					retrieve_item_ptr( table_list );

	return 1;
}


int compare_table_key( l, table_key )
LANGUAGE_TABLE *l;
char *table_key;
{
	return strcmp( l->table_key, table_key );
}

search_current_table_key()
{
	if ( !go_head( table_list ) )
	{
		fprintf( error_file, 
		"You must first set_table_key before issuing this command.\n" );
		exit( 1 );
	}
	if ( !item_exists( table_list, current_table_key, compare_table_key ) )
	{
		fprintf( error_file,
	"Internal error: search_current_table_key - table_key not found.\n" );
		exit( 1 );
	}

	current_language_table_ptr = (LANGUAGE_TABLE *)
					retrieve_item_ptr( table_list );

	return 1;
}



int set_uid_pwd( uid_pwd )
char *uid_pwd;
{
	table_load_uid_pwd( 	current_language_table_ptr->table_load, 
				uid_pwd );
	return 1;
}


int set_select_stmt( select_stmt )
char *select_stmt;
{
	char select_stmt_with_variables_replaced[ MAX_LINE_LENGTH ];
	char *replace_variables();

	replace_variables(	select_stmt_with_variables_replaced,
				select_stmt );

	return table_load_select(	current_language_table_ptr->table_load,
					select_stmt_with_variables_replaced );
}

int set_select_column( column )
char *column;
{
	return table_load_column_into(	current_language_table_ptr->table_load,
					column );
}



int load_the_table()
{
	return table_load_go( current_language_table_ptr->table_load );

}


/* --------------------------------------------------------------------
Example:
              Column 0                                       Column 4
	      --------		               		     --------
output_column invest_account_num where trading_account_num = 37-17738-7
----------------------------------------------------------------------- */
int output_column( string )
char *string;
{
	char column_to_output[ 50 ];
	char search_value[ 50 ];

	if ( !column( column_to_output, 0, string ) )
	{
		fprintf( error_file, 
			 "output_column: parameter line is blank.\n" );
		exit( 1 );
	}
	else		
	if ( !column( search_value, 4, string ) )
	{
		fprintf( error_file, 
			 "output_column: there is no search value.\n" );
		exit( 1 );
	}

	table_load_output_column(	current_language_table_ptr->table_load,
					column_to_output,
					search_value );

	return 1;
}


output_message( msg )
char *msg;
{
	char msg_with_variables_replaced[ MAX_LINE_LENGTH ];
	char *replace_variables();

	replace_variables(	msg_with_variables_replaced,
				msg );

	fprintf( stderr, "%s\n", msg_with_variables_replaced );
	fflush( stderr );
}

set_debug_mode()
{
	if ( !current_language_table_ptr )
	{
		fprintf( error_file, 
		"You must set_table_key before turning on debug mode.\n" );
		exit( 1 );
	}

	debug_mode = 1;
	table_load_set_debug_mode( current_language_table_ptr->table_load );

	return 1;
}

/* --------------------------------------------------------------
Example:
column2variable tradacct.rep_num 
	        to new_rep_num 
		where trading_account_num = E5-13013;
or
column2variable tradacct.rep_num 
	        to new_rep_num 
		where trading_account_num = :trading_account_num;
----------------------------------------------------------------- */
column2variable( string )
char *string;
{
	char column_0[ 80 ];
	char table_key[ 50 ];
	char column_to_copy[ 50 ];
	char into_variable[ 50 ];
	char search_value[ 50 ];
	char results[ 128 ];
	int return_value;
	int duplicate_indicator;

	if ( !column( column_0, 0, string ) )
	{
		fprintf( error_file, 
			 "column2variable: parameter line is blank.\n" );
		exit( 1 );
	}
	else		
	if ( !column( into_variable, 2, string ) )
	{
		fprintf( error_file, 
		"column2variable: there is no variable to copy into.\n" );
		exit( 1 );
	}
	if ( !column( search_value, 6, string ) )
	{
		fprintf( error_file, 
		"column2variable: where field_name = ??????.\n" );
		exit( 1 );
	}

	/* ------------------------------------------- */
	/* If search_value is preceeded with a colon,  */
	/* then find the data the variable represents. */
	/* ------------------------------------------- */
	if ( *search_value == ':' )
	{
		strcpy( search_value, 
			table_retrieve(	variable_table,
					search_value + 1,
					"data",
					&duplicate_indicator ) );
	}

	/* Example: column_0 = "tradacct.rep_num" */
	/* -------------------------------------- */
	piece( table_key, '.', column_0, 0 );
	current_table_key = table_key;
	search_current_table_key();

	if ( !piece( column_to_copy, '.', column_0, 1 ) )
	{
		fprintf( error_file,
		"column2variable: the syntax is table_key.column_to_copy\n" );
		exit( 1 );
	}

	return_value = table_load_get_column(	
				current_language_table_ptr->table_load,
				results,
				column_to_copy,
				search_value,
				&duplicate_indicator,
				error_file );

	/* If key not found in table */
	/* ------------------------- */
	if ( return_value == -1 )
	{
		if ( to_exception_upon_failure )
		{
			this_record_failed_column2variable = 1;
		}
		return 1;
	}

	/* ------------------------------------------------------------ */
	/* Three things have to happen if defaulting upon duplicate key */
	/* 1) populate_upon_duplicate must be called at least once.     */
	/* 2) "populate_upon_duplicate off" has not been done           */
	/* 3) The duplicate indicator must be true			*/
	/* ------------------------------------------------------------ */
	if ( text_to_populate_if_duplicate_found
	&&   strcmp( text_to_populate_if_duplicate_found, "off" ) != 0 
	&&   duplicate_indicator )
	{
		strcpy( results, text_to_populate_if_duplicate_found );
	}

	table_populate(	variable_table, 
			"variable",
			into_variable,
			"data",
			results );
	return 1;
}

set_input_file( filename )
char *filename;
{
	input_file = fopen( filename, "r" );
	if ( !input_file )
	{
		fprintf( error_file, 
			 "set_input_file: Cannot open: %s\n", 
			 filename );
		exit( 1 );
	}

	return 1;
}

set_output_file( filename )
char *filename;
{
	output_file = fopen( filename, "w" );
	if ( !output_file )
	{
		fprintf( error_file, 
			 "set_output_file: Cannot open: %s\n", 
			 filename );
		exit( 1 );
	}

	return 1;
}

set_exception_file( filename )
char *filename;
{
	exception_file = fopen( filename, "w" );
	if ( !exception_file )
	{
		fprintf( error_file, 
			 "set_exception_file: Cannot open: %s\n", 
			 filename );
		exit( 1 );
	}

	return 1;
}

set_error_file( filename )
char *filename;
{
	error_file = fopen( filename, "w" );
	if ( !error_file )
	{
		fprintf( stderr, 
			 "set_error_file: Cannot open: %s\n", 
			 filename );
		exit( 1 );
	}

	return 1;
}


for_each_row()
{
	/* If there is no set_input_file statement. */
	/* ---------------------------------------- */
	if ( !input_file ) input_file = stdin;

	/* If there is no input_record_delimiter, then make it EOLN */
	/* -------------------------------------------------------- */
	if ( !input_record_delimiter ) input_record_delimiter = EOLN;

	/* Get a record of input and place to package variable: input_string. */
	/* ------------------------------------------------------------------ */
	if( !get_record_of_input( input_string, input_file ) )
	{
		eof_indicator = 1;
	}
	else
	{
		this_record_failed_column2variable = 0;
		clean_out_record();
	}
	return 1;
}



end_for()
{
	return 1;
}




/* Clean out any embedded <CR> <LF> in record (if any) */
/* --------------------------------------------------- */
clean_out_record()
{
	char *in_ptr;
	char *out_ptr;
	char tmp[ MAX_LINE_LENGTH + 1 ];

	in_ptr = input_string;
	out_ptr = tmp;

	while( *in_ptr )
	{
		if ( *in_ptr != 10 && *in_ptr != 13 )
			*out_ptr++ = *in_ptr;

		in_ptr++;
	}

	*out_ptr = '\0';
	strcpy( input_string, tmp );
	return 1;
}



int get_record_of_input( input_string, input_file )
char *input_string;
FILE *input_file;
{
	int c;
	int byte_count = 0;
	char *ptr = input_string;

	*ptr = '\0';
	while( ( c = getc( input_file ) ) != EOF )
	{
		/* If all done */
		/* ----------- */
		if ( end_of_file_marker
		&&   end_of_file_marker == c )
		{
			return 0;
		}

		if ( ++byte_count == MAX_LINE_LENGTH )
		{
			fprintf( error_file, 
			"get_record_of_input: MAX_LINE_LENGTH exceeded.\n" );
			exit( 1 );
		}

		/* If End of Record */
		/* ---------------- */
		if ( c == input_record_delimiter )
		{
			*ptr = '\0';
			return 1;
		}
		else
			*ptr++ = (char)c;
	}

	if ( end_of_file_marker )
	{
		fprintf( error_file, 
       "get_record_of_input: NO end of file marker. Probably INCOMPLETE!!!\n" );
		exit( 1 );
	}

	/* Otherwise, all done */
	/* ------------------- */
	return 0;
}




/* ------------------------
Example:
	     Column 0
	     | Column 1
	     | |
	     V V
set_variable 0 primary_key;
set_variable 1 field_1;
--------------------------- */
int set_variable( piece_variable )
char *piece_variable;
{
	char piece_str[ 10 ];
	char variable[ 50 ];
	char data[ 1024 ];

	/* If did not set_input_field_delimiter then make it '|' */
	/* ----------------------------------------------------- */
	if ( !input_field_delimiter ) input_field_delimiter = '|';

	if ( !column( piece_str, 0, piece_variable ) )
	{
		fprintf( error_file, 
"set_variable: need to specify which piece and variable to store data into.\n");
		exit( 1 );
	}

	if ( !isdigit( *piece_str ) )
	{
		fprintf( error_file,
"set_variable: need to specify which piece to store data into.\n" );
		exit( 1 );
	}

	if ( !column( variable, 1, piece_variable ) )
	{
		fprintf( error_file, 
"set_variable: need to specify a variable name to store data into.\n" );
		exit( 1 );
	}

	if ( !piece(	data,
			input_field_delimiter,
			input_string, 
			atoi( piece_str ) ) )
	{
		fprintf( error_file,
"set_variable: You ran out of columns to set this variable: (%s)(%s).\n",
			variable,
			input_string );

		exit( 1 );
	}

	table_populate(	variable_table, 
			"variable",
			variable,
			"data",
			data );

	return 1;
}


int output_variable( variable_list )
char *variable_list;
{
	char variable[ 50 ];
	char *data;
	int i, duplicate_indicator;

	/* If there is no set_output_file statement. */
	/* ----------------------------------------- */
	if ( !output_file ) output_file = stdout;

	/* If a select failed and we're sending to an execption file */
	/* --------------------------------------------------------- */
	if ( this_record_failed_column2variable && exception_file )
	{
		if ( input_record_delimiter != EOLN )
		{
			fprintf( exception_file, 
			 	"%s%c\n", 
			 	input_string, 
			 	input_record_delimiter );
		}
		else
		{
			fprintf( exception_file, 
			 	"%s\n", 
			 	input_string );
		}

		fflush( exception_file );
		return 1;
	}

	/* Otherwise, do the normal output */
	/* ------------------------------- */
	for( i = 0; column( variable, i, variable_list ); i++ )
	{
		data = table_retrieve(	variable_table,
					variable,
					"data",
					&duplicate_indicator );

		if ( !data )
		{
			fprintf( error_file,
				 "output_variable: (%s) NOT FOUND\n", 
				 variable );
			exit( 1 );
		}

		if ( i == 0 )
		{
			if ( debug_mode ) fprintf( error_file, "%s", data );

			fprintf( output_file, "%s", data );
		}
		else
		{
			if ( debug_mode ) fprintf( error_file, "|%s", data );

			fprintf( output_file, "|%s", data );
		}
	}
	
	if ( debug_mode ) fprintf( error_file, "\n" );

	fprintf( output_file, "\n" );

	return 1;
}


/* ---------------------------------
Example:
       Column 0   1 2 3  4
       --------   - - -- ---------
substr long_field 0 2 to sub_field1;
------------------------------------ */
int loadlang_substr( substr_argument_line )
char *substr_argument_line;
{
	char source_variable[ 50 ];
	char *source_data;
	char starting_offset_str[ 50 ];
	char number_of_characters_to_copy[ 50 ];
	char destination_variable[ 50 ];
	char destination_data[ 1024 ];
	int duplicate_indicator;

	if ( !column( source_variable, 0, substr_argument_line ) )
	{
		fprintf( error_file, "substr: parameter line is blank.\n" );
		exit( 1 );
	}

	if ( !column( starting_offset_str, 1, substr_argument_line ) )
	{
		fprintf( error_file, "substr: not enough parameters.\n" );
		exit( 1 );
	}

	if ( !isdigit( *starting_offset_str ) )
	{
		fprintf( error_file, 
			 "substr: the starting offset must be a number.\n" );
		exit( 1 );
	}

	if ( !column( number_of_characters_to_copy, 2, substr_argument_line ) )
	{
		fprintf( error_file, "substr: not enough parameters.\n" );
		exit( 1 );
	}

	if ( !isdigit( *number_of_characters_to_copy ) )
	{
		fprintf( error_file, 
		"substr: the number of characters to copy must be a number.\n");
		exit( 1 );
	}

	if ( !column( destination_variable, 4, substr_argument_line ) )
	{
		fprintf( error_file, "substr: parameter line is blank.\n" );
		exit( 1 );
	}

	/* Get the data of the source_variable */
	/* ----------------------------------- */
	source_data = table_retrieve(	variable_table,
					source_variable,
					"data",
					&duplicate_indicator );

	if ( !source_data )
	{
		fprintf( error_file,
			 "substr: (%s) NOT FOUND\n", 
			 source_variable );
		exit( 1 );
	}

	/* Perform the substr copy */
	/* ----------------------- */
	strncpy(	destination_data, 
			source_data + atoi( starting_offset_str ),
			atoi( number_of_characters_to_copy ) );

	/* Need to put the null in since strncpy might not. */
	/* ------------------------------------------------ */
	*( destination_data + atoi( number_of_characters_to_copy ) ) = '\0';

	/* Set the destination variable in place */
	/* ------------------------------------- */
	table_populate(	variable_table, 
			"variable",
			destination_variable,
			"data",
			destination_data );
	return 1;
}


/* ------------------------------
Example:
	     Column 0    Column 1
	     --------    --------
set_constant sponsor_num KM;
---------------------------------- */
int set_constant( variable_constant )
char *variable_constant;
{
	char variable[ 50 ];
	char constant[ 80 ];

	if ( !column( variable, 0, variable_constant ) )
	{
		fprintf( error_file, 
			 "set_constant: parameter line is blank.\n" );
		exit( 1 );
	}

	/* If this fails then user wants to zap a variable */
	/* ----------------------------------------------- */
	if ( !column( constant, 1, variable_constant ) )
		*constant = '\0';

	table_populate(	variable_table, 
			"variable",
			variable,
			"data",
			constant );

	return 1;
}

int if_strcmp_is_true_continue( this_that_string )
char *this_that_string;
{
	char this[ 80 ];
	char that[ 80 ];

	split_this_with_that( this, that, this_that_string );

	if ( strcmp( this, that ) == 0 )
	{
		return process_continue();
	}

	return 1;
}

int if_strcmp_is_false_continue( this_that_string )
char *this_that_string;
{
	char this[ 80 ];
	char that[ 80 ];

	split_this_with_that( this, that, this_that_string );

	if ( strcmp( this, that ) != 0 )
	{
		return process_continue();
	}

	return 1;
}

split_this_with_that( this, that, source )
char *this;
char *that;
char *source;
{
	char this_that_with_variables_replaced[ 128 ];

	replace_variables(	this_that_with_variables_replaced,
				source );

	if ( !column( this, 0, this_that_with_variables_replaced ) )
	{
		fprintf( error_file, 
		"if_strcmp_is_?_continue: parameter line is blank.\n" );
		exit( 1 );
	}

	if ( !column( that, 1, this_that_with_variables_replaced ) )
	{
		fprintf( error_file, 
		"if_strcmp_is_?_continue: invalid parameter line: %s\n",
			 source );
		exit( 1 );
	}
}

process_continue()
{
	program_counter = search_for_end_for( program_counter );

	if ( program_counter == -1 )
	{
		fprintf( stderr, 
		"ERROR loadlang.e: there is no \"end_for\" to continue to.\n" );
		exit( 1 );
	}
}


int set_input_field_delimiter( delimiter )
char *delimiter;
{
	if ( !delimiter )
	{
		fprintf( error_file, 
		"set_input_field_delimiter: no argument passed.\n" );
		exit( 1 );
	}
	input_field_delimiter = *delimiter;
	return 1;
}

int set_input_record_delimiter( delimiter )
char *delimiter;
{
	if ( !delimiter )
	{
		fprintf( error_file, 
		"set_input_record_delimiter: no argument passed.\n" );
		exit( 1 );
	}

	if ( strcmp( delimiter, "EOLN" ) == 0 )
		input_record_delimiter = EOLN;
	else
		input_record_delimiter = *delimiter;
	return 1;
}

int set_end_of_file_marker( delimiter )
char *delimiter;
{
	if ( !delimiter )
	{
		fprintf( error_file, 
		"set_end_of_file_marker: no argument passed.\n" );
		exit( 1 );
	}
	end_of_file_marker = *delimiter;
	return 1;
}

/* ----------------------------------------
Example:
# ----------------------------------------;
# trading_account_num looks like: E5 00000;
# Needs to look like:             E5-00000;
# ----------------------------------------;
set_byte trading_account_num - 2;
------------------------------------------- */
int set_byte( parameter_line )
char *parameter_line;
{
	char variable[ 50 ];
	char *old_data;
	char new_data[ 1024 ];
	char byte_str[ 50 ];
	char offset_str[ 50 ];
	int offset;
	int str_len;
	int duplicate_indicator;

	if ( !column( variable, 0, parameter_line ) )
	{
		fprintf( error_file, "set_byte: parameter line is blank.\n" );
		exit( 1 );
	}

	if ( !column( byte_str, 1, parameter_line ) )
	{
		fprintf( error_file, 
			 "set_byte: usage variable byte offset.\n" );
		exit( 1 );
	}

	if ( !column( offset_str, 2, parameter_line ) )
	{
		fprintf( error_file, 
			 "set_byte: usage variable byte offset.\n" );
		exit( 1 );
	}

	old_data = table_retrieve(	variable_table,
					variable,
					"data",
					&duplicate_indicator );

	if ( !old_data )
	{
		fprintf( error_file,
			 "set_byte: (%s) NOT FOUND\n", 
			 variable );
		exit( 1 );
	}

	str_len = strlen( old_data );
	offset = atoi( offset_str );

	/* If place within string */
	/* ---------------------- */
	if ( str_len > offset )
	{
		strcpy( new_data, old_data );
		new_data[ offset ] = *byte_str;
	}
	else
	/* ------------------------------- */
	/* Otherwise, must pad with spaces */
	/* ------------------------------- */
	{
		sprintf( new_data, "%-*s%c", offset, old_data, *byte_str );
	}

	table_populate(	variable_table, 
			"variable",
			variable,
			"data",
			new_data );

	return 1;
}


set_first_line_into( variable )
char *variable;
{
	/* If there is no set_input_file statement. */
	/* ---------------------------------------- */
	if ( !input_file ) input_file = stdin;

	if ( !fgets( input_string, MAX_LINE_LENGTH - 1, input_file ) )
	{
		fprintf( error_file,
			 "set_first_line_into: input file is empty.\n" );
		exit( 1 );
	}

	trim( input_string );

	table_populate(	variable_table, 
			"variable",
			variable,
			"data",
			input_string );

	return 1;
}


/* ----------------------------------------------------
Example:
concatenate mail_hold_sec_id_1 behind mail_hold_sec_id;
------------------------------------------------------- */
int concatenate( parameter_line )
char *parameter_line;
{
	char variable_source[ 50 ];
	char variable_behind[ 50 ];
	char *data_source_ptr;
	char *data_behind_ptr;
	char new_data_behind[ 1024 ];
	int duplicate_indicator;

	if ( !column( variable_source, 0, parameter_line ) )
	{
		fprintf( error_file, 
			 "concatenate: parameter line is blank.\n" );
		exit( 1 );
	}

	if ( !column( variable_behind, 2, parameter_line ) )
	{
		fprintf( error_file, 
			 "concatenate: incomplete parameter line.\n" );
		exit( 1 );
	}

	data_source_ptr = table_retrieve(	variable_table,
						variable_source,
						"data",
						&duplicate_indicator );
	if ( !data_source_ptr )
	{
		fprintf( error_file,
			 "concatenate: (%s) NOT FOUND\n", 
			 variable_source );
		exit( 1 );
	}

	data_behind_ptr = table_retrieve(	variable_table,
						variable_behind,
						"data",
						&duplicate_indicator );
	if ( !data_behind_ptr )
	{
		fprintf( error_file,
			 "concatenate: (%s) NOT FOUND\n", 
			 variable_behind );
		exit( 1 );
	}

	sprintf( new_data_behind, 
		 "%s%s",
		 data_behind_ptr,
		 data_source_ptr );

	table_populate(	variable_table, 
			"variable",
			variable_behind,
			"data",
			new_data_behind );
	return 1;
}


int output_exception_file( parameter_line )
char *parameter_line;
{
	char variable[ 50 ];
	char *data;
	int duplicate_indicator;

	if ( !column( variable, 0, parameter_line ) )
	{
		fprintf( error_file, 
			 "output_exception_file: parameter line is blank.\n" );
		exit( 1 );
	}

	data = table_retrieve(	variable_table,
				variable,
				"data",
				&duplicate_indicator );
	if ( !data )
	{
		fprintf( error_file,
			 "output_exception_file: (%s) NOT FOUND\n", 
			 variable );
		exit( 1 );
	}

	if ( !exception_file )
	{
		fprintf( error_file,
		 "output_exception_file: need to set_exception_file first.\n" );
		 exit( 1 );
	}

	fprintf( exception_file, "%s\n", data );
	fflush( exception_file );

	return 1;
}

binary_operation(operation, variable_list)
char *operation;
char *variable_list;
{
	char variable1[ 50 ];
	char variable2[ 50 ];
	char variable3[ 50 ];
	char result[ 80 ];
	char *variable1_ptr;
	char *variable2_ptr;
	float num1, num2, num3;
	int duplicate_indicator;

	if ( !column( variable1, 0, variable_list ) )
	{
		fprintf(error_file, 
			"%s: parameter line is blank.\n", 
			operation );
		exit( 1 );
	}
	if ( !column( variable2, 1, variable_list ) )
	{
		fprintf(error_file, 
			"%s: parameter is not correct.\n",
			operation );
		exit( 1 );
	}
	if ( !column( variable3, 3, variable_list ) )
	{
		fprintf(error_file, 
			"%s: parameter is not correct.\n",
			operation );
		exit( 1 );
	}
	/* Get the data of the variables */
	/* ----------------------------- */
	variable1_ptr = table_retrieve(	variable_table,
					variable1,
					"data",
					&duplicate_indicator );

	if ( !variable1_ptr )
	{
		fprintf(error_file,
			"%s: (%s) NOT FOUND\n", 
			operation,
			variable1 );
		exit( 1 );
	}
	variable2_ptr = table_retrieve(	variable_table,
					variable2,
					"data",
					&duplicate_indicator );

	if ( !variable2_ptr )
	{
		fprintf(error_file,
			"%s: (%s) NOT FOUND\n", 
			operation,
			variable2 );
		exit( 1 );
	}

	num1 = atof(variable1_ptr);
	num2 = atof(variable2_ptr);

	if (strcmp(operation, "multiply") == 0)	{
		num3 = num1 * num2;
		sprintf(result, "%.2f", num3);
	}
	else if (strcmp(operation, "add") == 0)	{
		num3 = num1 + num2;
		sprintf(result, "%.2f", num3);
	}
	else if (strcmp(operation, "subtract") == 0)  {
		num3 = num1 - num2;
		sprintf(result, "%.2f", num3);
	}
	else if (strcmp(operation, "divide") == 0)  {
		num3 = num1 / num2;
		sprintf(result, "%.3f", num3);
	}
	else
	{
		fprintf(error_file, 
		     	"binary_operation: Operation is not correct: %s.\n",
			operation );
		exit( 1 );
	}


	table_populate(	variable_table, 
			"variable",
			variable3,
			"data",
			result );

	return 1;
}

/* ------------------------------
Example:
	Column 0  1     2      3
    	-------- ----  ---- ------   
multiply field1 field2 into field3;
---------------------------------- */
int multiply( variable_list )
char *variable_list;
{
	binary_operation("multiply", variable_list);

	return 1;
}

int add( variable_list )
char *variable_list;
{
	binary_operation("add", variable_list);

	return 1;
}

int subtract( variable_list )
char *variable_list;
{
	binary_operation("subtract", variable_list);

	return 1;
}

int divide( variable_list )
char *variable_list;
{
	binary_operation("divide", variable_list);

	return 1;
}

/* ----------------------------
Example:
set_sysdate TODAY;
---------------------------- */
int set_sysdate( variable_date )
char *variable_date;
{
	char variable[ 50 ];
	char sysdate[50];

	if ( !column( variable, 0, variable_date ) )
	{
		fprintf( error_file, 
			 "set_sysdate: parameter line is blank.\n" );
		exit( 1 );
	}

	strcpy(sysdate, current_date() );
	
	table_populate(	variable_table, 
			"variable",
			variable,
			"data",
			sysdate );

	return 1;
}


int set_last_work_date_month( variable_date )
char *variable_date;
{
	char variable[ 50 ];
	char last_workday[50];

	if ( !column( variable, 0, variable_date ) )
	{
		fprintf( error_file, 
			"last_work_date_month: parameter line is blank.\n" );
		exit( 1 );
	}

	strcpy(last_workday, last_workday_of_previous_month() );
	
	table_populate(	variable_table, 
			"variable",
			variable,
			"data",
			last_workday );

	return 1;
}



/* ---------------------
Example:
set_argument 1 batch_id;
------------------------ */
int set_argument( argument_data )
char *argument_data;
{
	char variable[ 50 ];
	char argc_str[50];
	int argc_to_use;

	if ( !column( argc_str, 0, argument_data ) )
	{
		fprintf( error_file, 
			 "set_argument: parameter line is blank.\n" );
		exit( 1 );
	}

	if ( !column( variable, 1, argument_data ) )
	{
		fprintf( error_file, 
			 "set_argument: variable is missing.\n" );
		exit( 1 );
	}

	argc_to_use = atoi( argc_str ) + 1;
	if ( !global_argv[ argc_to_use ] )
	{
		fprintf( error_file, 
"set_argument: Not enough command line arguments to (set_argument %s)\n",
			 argument_data );
		exit( 1 );
	}

	table_populate(	variable_table, 
			"variable",
			variable,
			"data",
			global_argv[ argc_to_use ] );

	return 1;

}

int send_to_exception_upon_failure( yes_or_no )
char *yes_or_no;
{
	if ( stricmp( yes_or_no, "yes" ) == 0 )
		to_exception_upon_failure = 1;
	else
	if ( stricmp( yes_or_no, "no" ) == 0 )
		to_exception_upon_failure = 0;
	else
	{
		fprintf(error_file,
			"send_to_exception_upon_failure: yes or no\n" );
		exit( 1 );
	}

	return 1;
}


populate_upon_duplicate( text )
char *text;
{
	/* If called previously then free current memory */
	/* --------------------------------------------- */
	if ( text_to_populate_if_duplicate_found )
		free( text_to_populate_if_duplicate_found );

	text_to_populate_if_duplicate_found = strdup( text );
	return 1;
}




char *replace_variables( destination, source )
char *destination;
char *source;
{
	char *s = source;
	char *d = destination;

	while( *s )
	{
		/* --------------------------------------- */
		/* If not a colon followed by a non space, */
		/* then simply copy it over.		   */
		/* --------------------------------------- */
		if ( ( *s != ':' ) || ( isspace( *(s + 1) ) ) )
		{
			*d++ = *s++;
		}
		else
		/* ----------------------------------- */
		/* Otherwise, extract the variable and */
		/* convert it to it's contents.        */
		/* ----------------------------------- */
		{
			char variable[ 128 ];
			char *variable_ptr = variable;
			char *contents;
			int duplicate_indicator;

			/* Copy to variable */
			/* ---------------- */
			s++;
			while( *s )
				if ( isalnum( *s ) 
				||   *s == '_' )
					*variable_ptr++ = *s++;
				else
					break;

			*variable_ptr = '\0';

			/* Get the contents */
			/* ---------------- */
			contents = table_retrieve(	variable_table,
							variable,
							"data",
							&duplicate_indicator );

			if ( !contents )
			{
				fprintf(error_file,
					"Variable: (%s) NOT FOUND\n", 
					variable );
				exit( 1 );
			}

			/* Copy the variable contents to the buffer */
			/* ---------------------------------------- */
			while( *contents ) *d++ = *contents++;
		}
	}

	*d = '\0';
	return d;
}
