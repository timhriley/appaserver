/* $APPASERVER_HOME/library/generic_load.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "piece.h"
#include "column.h"
#include "date_convert.h"
#include "document.h"
#include "dictionary.h"
#include "date.h"
#include "timlib.h"
#include "folder.h"
#include "relation.h"
#include "appaserver_error.h"
#include "role.h"
#include "appaserver_parameter.h"
#include "role_folder.h"
#include "form.h"
#include "frameset.h"
#include "element.h"
#include "row_security.h"
#include "generic_load.h"

GENERIC_LOAD *generic_load_new( void )
{
	GENERIC_LOAD *generic_load;

	generic_load = (GENERIC_LOAD *)calloc( 1, sizeof( GENERIC_LOAD ) );
	if ( !generic_load )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	return generic_load;
}

GENERIC_LOAD_FOLDER *generic_load_folder_new( char *folder_name )
{
	GENERIC_LOAD_FOLDER *generic_load_folder;

	generic_load_folder =
		(GENERIC_LOAD_FOLDER *)
			calloc( 1, sizeof( GENERIC_LOAD_FOLDER ) );
	if ( !generic_load_folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	generic_load_folder->folder_name = folder_name;
	generic_load_folder->generic_load_attribute_list = list_new();
	generic_load_folder->primary_key_date_offset = -1;
	generic_load_folder->primary_key_time_offset = -1;

	return generic_load_folder;
}

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_new(
			char *attribute_name,
			boolean is_primary_key )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	generic_load_attribute =
		(GENERIC_LOAD_ATTRIBUTE *)
			calloc( 1, sizeof( GENERIC_LOAD_ATTRIBUTE ) );
	if ( !generic_load_attribute )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	generic_load_attribute->attribute_name = attribute_name;
	generic_load_attribute->is_primary_key = is_primary_key;
	return generic_load_attribute;
}

#ifdef NOT_DEFINED
FOLDER *generic_load_get_database_folder(
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			boolean with_mto1_related_folders )
{
	FOLDER *folder;
	ROLE *role;

	role = role_new_role(	application_name,
				role_name );

	folder = folder_new_folder(
				application_name, 
				session,
				folder_name );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->post_change_javascript,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			&folder->create_view_statement,
			application_name,
			session,
			folder->folder_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	folder->attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	if ( with_mto1_related_folders )
	{
		folder->mto1_related_folder_list =
			related_folder_get_mto1_related_folder_list(
				list_new_list(),
				application_name,
				session,
				folder_name,
				role_name,
				0 /* isa_flag */,
				related_folder_recursive_all,
				0 /* override_row_restrictions */,
				(LIST *)0 /* root_primary_att..._name_list */,
				0 /* recursive_level */ );
	}

	return folder;
}
#endif

LIST *generic_load_get_folder_list(
			FOLDER *folder,
			DICTIONARY *position_dictionary,
			DICTIONARY *constant_dictionary,
			char *login_name )
{
	LIST *generic_load_folder_list;
	RELATED_FOLDER *related_folder;
	GENERIC_LOAD_FOLDER *generic_load_folder;

	if ( !position_all_valid( position_dictionary ) )
	{
		return (LIST *)0;
	}

	if ( ! ( generic_load_folder =
			generic_load_get_folder(
				folder->folder_name,
				folder->attribute_list,
				position_dictionary,
				constant_dictionary,
				0 /* not primary_attibutes_only */,
				login_name,
				(char *)0 /* related_attribute_name */ ) ) )
	{
		return (LIST *)0;
	}

	generic_load_get_primary_key_date_time_offset(
		&generic_load_folder->primary_key_date_offset,
		&generic_load_folder->primary_key_time_offset,
		generic_load_folder->
			generic_load_attribute_list );

	generic_load_folder_list = list_new();

	list_append_pointer(	generic_load_folder_list,
				generic_load_folder );

	if ( list_rewind( folder->mto1_related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					folder->mto1_related_folder_list );

			if ( ! ( generic_load_folder =
				generic_load_get_folder(
					related_folder->folder->folder_name,
					related_folder->
						folder->
						attribute_list,
					position_dictionary,
					constant_dictionary,
					1 /* primary_attibutes_only */,
					login_name,
					related_folder->
						related_attribute_name ) ) )
			{
/*
				char msg[ 256 ];
				sprintf( msg,
"ERROR in %s/%s()/%d: cannot get generic_load_folder for mto1 related_folder = (%s)\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					related_folder->folder->folder_name );
				m2( application_name, msg );

				return (LIST *)0;
*/
				continue;
			}

			list_append_pointer(	generic_load_folder_list,
						generic_load_folder );

		} while( list_next( folder->mto1_related_folder_list ) );
	}

	return generic_load_folder_list;

}

GENERIC_LOAD_FOLDER *generic_load_get_folder(
				char *folder_name,
				LIST *attribute_list,
				DICTIONARY *position_dictionary,
				DICTIONARY *constant_dictionary,
				boolean primary_attributes_only,
				char *login_name,
				char *related_attribute_name )
{
	GENERIC_LOAD_FOLDER *generic_load_folder = {0};
	ATTRIBUTE *attribute;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	int position_integer;
	char *constant_string;

	if ( !list_rewind( attribute_list ) )
	{
		return (GENERIC_LOAD_FOLDER *)0;
	}

	do {
		attribute = list_get_pointer( attribute_list );

		if ( primary_attributes_only
		&&   !attribute->primary_key_index )
		{
			continue;
		}

		if ( strcmp( attribute->attribute_name, "login_name" ) == 0
		&& ( attribute->omit_insert
		||   attribute->omit_insert_prompt ) )
		{
			constant_string = login_name;
		}
		else
		{
			constant_string =
				dictionary_get_pointer(
					constant_dictionary,
					attribute->attribute_name );
		}

		if ( constant_string )
		{
			generic_load_attribute =
				generic_load_attribute_new(
					attribute->attribute_name,
					attribute->primary_key_index );
			generic_load_attribute->constant = constant_string;
		}
		else
		{
			if ( ! ( position_integer =
				generic_load_get_position_integer(
					position_dictionary,
					attribute->attribute_name,
					related_attribute_name ) ) )
			{
				continue;
			}

			generic_load_attribute =
				generic_load_attribute_new(
					attribute->attribute_name,
					attribute->primary_key_index );
	
			generic_load_attribute->position = position_integer;
			generic_load_attribute->datatype = attribute->datatype;

			if ( attribute->primary_key_index
			&& (strcmp( attribute->datatype, "date" ) == 0
			|| strcmp( attribute->datatype, "current_date" )==0))
			{
				generic_load_attribute->
					is_primary_key_date_datatype = 1;
			}
			else
			if ( attribute->primary_key_index
			&& ( strcmp(	attribute->datatype,
					"time" ) == 0
			||   strcmp(	attribute->datatype,
					"current_time" ) == 0 ) )
			{
				generic_load_attribute->
					is_primary_key_time_datatype = 1;
			}
		}

		if ( !generic_load_folder )
		{
			generic_load_folder =
				generic_load_folder_new(
					folder_name );
		}

		list_append_pointer(	generic_load_folder->
						generic_load_attribute_list,
					generic_load_attribute );

	} while( list_next( attribute_list ) );

	return generic_load_folder;

}

void generic_load_replace_time_2400_with_0000(
				char *input_buffer,
				char delimiter,
				int primary_key_date_offset,
				int primary_key_time_offset )
{
	char time_string[ 16 ];
	char date_string[ 16 ];
	DATE *date = {0};

	if ( primary_key_date_offset == -1
	||   primary_key_time_offset == -1 )
	{
		return;
	}

	if ( !piece(	time_string,
			delimiter,
			input_buffer,
			primary_key_time_offset ) )
	{
		return;
	}

	if ( strcmp( time_string, "2400" ) != 0 ) return;

	if ( !piece(	date_string,
			delimiter,
			input_buffer,
			primary_key_date_offset ) )
	{
		return;
	}

	if ( !date )
	{
		date = date_yyyy_mm_dd_new( date_string );
	}
	else
	{
		date_set_yyyy_mm_dd( date, date_string );
	}

	date_increment_days( date, 1.0 );

	piece_replace(	input_buffer,
			delimiter, 
			date_display_yyyy_mm_dd( date ),
			primary_key_date_offset );

	piece_replace(	input_buffer,
			delimiter, 
			"0000",
			primary_key_time_offset );

}

boolean generic_load_fix_time(
				char *input_buffer,
				char delimiter,
				int primary_key_time_offset )
{
	char *time_string;
	boolean replace_back = 0;

	if ( primary_key_time_offset == -1 )
	{
		return 1;
	}

	time_string =
		generic_load_get_piece_buffer(
			input_buffer,
			&delimiter,
			(char *)0 /* application_name */,
			(char *)0 /* datatype */,
			(char *)0 /* constant */,
			primary_key_time_offset,
			1 /* is_primary_key */ );

	if ( character_exists( time_string, ':' ) )
	{
		trim_character( time_string, ':', time_string );
		replace_back = 1;
	}

	if ( timlib_pad_time_four( time_string ) )
		replace_back = 1;

	if ( !timlib_is_valid_time( time_string ) ) return 0;

	if ( replace_back )
	{
		piece_replace(	input_buffer,
				delimiter, 
				time_string,
				primary_key_time_offset );
	}
	return 1;
}

void generic_load_get_primary_key_date_time_offset(
			int *primary_key_date_offset,
			int *primary_key_time_offset,
			LIST *generic_load_attribute_list )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( !list_rewind( generic_load_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty generic_load_attribute_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		generic_load_attribute =
			list_get_pointer( generic_load_attribute_list );

		if ( generic_load_attribute->is_primary_key
		&&   generic_load_attribute->is_primary_key_date_datatype
		&&   !generic_load_attribute->constant )
		{
			*primary_key_date_offset =
				generic_load_attribute->position - 1;
		}

		if ( generic_load_attribute->is_primary_key
		&&   generic_load_attribute->is_primary_key_time_datatype
		&&   !generic_load_attribute->constant )
		{
			*primary_key_time_offset =
				generic_load_attribute->position - 1;
		}

	} while( list_next( generic_load_attribute_list ) );

}

char *generic_load_get_piece_buffer(
			char *input_buffer,
			char *delimiter,
			char *application_name,
			char *datatype,
			char *constant,
			int piece_offset,
			boolean is_primary_key )
{
	static char piece_buffer[ 1024 ];

	*piece_buffer = '\0';

	if ( constant )
	{
		strcpy(	piece_buffer, constant );
	}
	else
	{
/*
		if ( !delimiter || !*delimiter )
		{
			strcpy( piece_buffer, input_buffer );
		}
		else
		if ( strcmp( delimiter, "spaces" ) == 0 )
		{
			column(	piece_buffer,
				piece_offset,
				input_buffer );
		}
		else
		if ( *delimiter == ',' )
		{
			piece_quoted(
				piece_buffer,
				*delimiter,
				input_buffer,
				piece_offset,
				'"' );
		}
		else
		{
			piece(	piece_buffer,
				*delimiter,
				input_buffer,
				piece_offset );
		}
*/
		piece_unknown(
			piece_buffer,
			delimiter,
			input_buffer,
			piece_offset );
	}

	if ( !is_primary_key
	&&   strcmp( piece_buffer, NULL_STRING ) == 0 )
	{
		strcpy( piece_buffer, DATABASE_NULL );
	}
	else
	if ( is_primary_key
	&&   !*piece_buffer )
	{
		strcpy( piece_buffer, NULL_STRING );
	}

	if ( *piece_buffer
	&&   strcmp( piece_buffer, DATABASE_NULL ) != 0
	&&   strcmp( piece_buffer, NULL_STRING ) != 0
	&&   datatype
	&&   application_name
	&& ( strcmp(	datatype,
			"date" ) == 0
	||   strcmp(	datatype,
			"current_date" ) == 0 ) )
	{
		DATE_CONVERT *date_convert;

		if ( date_convert_date_get_format( piece_buffer ) !=
		     international )
		{
			date_convert =
				date_convert_new_database_format_date_convert(
					application_name,
					piece_buffer );
	
			if ( !date_convert )
			{
				*piece_buffer = '\0';
			}
			else
			{
		 		strcpy(	piece_buffer,
					date_convert->return_date );
				date_convert_free( date_convert );
			}
		}
	}

	return piece_buffer;
}

boolean position_all_valid( DICTIONARY *position_dictionary )
{
	int array[ 1024 ] = {0};
	LIST *key_list;
	char *position_string;
	int position_integer;
	char *key;

	key_list = dictionary_get_key_list( position_dictionary );

	if ( !list_rewind( key_list ) ) return 0;

	do {
		key = list_get_pointer( key_list );
		position_string = dictionary_get_pointer(
					position_dictionary,
					key );

		position_integer = atoi( position_string );

		if ( position_integer <= 0 ) return 0;

		if ( position_integer > 1023 )
		{
			printf(
			"<h3>An exceedingly large position occurred.</h3>\n" );
			return 0;
		}

		if ( array[ position_integer - 1 ] )
		{
			printf( "<h3>Position duplication occurred.</h3>\n" );
			return 0;
		}

		array[ position_integer - 1 ] = 1;

	} while( list_next( key_list ) );

	return 1;

}

void generic_load_attribute_set_is_participating(
		LIST *generic_load_folder_list,
		char *delimiter,
		char *application_name,
		char *load_filename,
		int skip_header_rows )
{
	char input_buffer[ 4096 ];
	char *piece_buffer;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	FILE *input_file;
	LIST *generic_load_attribute_list;
	GENERIC_LOAD_FOLDER *generic_load_folder;

	if ( !load_filename
	||   !( input_file = fopen( load_filename, "r" ) ) )
	{
		printf( "<p>ERROR: please transmit a file.\n" );
		document_close();
		exit( 0 );
	}

	if ( !list_length( generic_load_folder_list ) )
	{
		printf( "<p>ERROR: cannot get folder to load.\n" );
		document_close();
		exit( 0 );
	}

	generic_load_reset_row_count();

	while( get_line( input_buffer, input_file ) )
	{
		trim( input_buffer );

		if ( generic_load_skip_header_rows(
			skip_header_rows ) )
		{
			continue;
		}

		if ( !*input_buffer ) continue;
		if ( *input_buffer == '#' ) continue;

		list_rewind( generic_load_folder_list );

		do {
			generic_load_folder =
				list_get_pointer(
					generic_load_folder_list );

			generic_load_attribute_list =
				generic_load_folder->
					generic_load_attribute_list;

			if ( !list_rewind( generic_load_attribute_list ) )
			{
				continue;
			}

			list_rewind( generic_load_attribute_list );
			do {
				generic_load_attribute =
					list_get_pointer(
						generic_load_attribute_list );

				piece_buffer =
				generic_load_get_piece_buffer(
					input_buffer,
					delimiter,
					application_name,
					generic_load_attribute->datatype,
					generic_load_attribute->
						constant,
					generic_load_attribute->
						position - 1,
					generic_load_attribute->
						is_primary_key );

				if ( *piece_buffer
				&&   strcmp( piece_buffer, NULL_STRING ) == 0
				&&   !list_at_head( generic_load_folder_list ) )
				{
					continue;
				}

				if ( *piece_buffer )
				{
					generic_load_attribute->
						is_participating = 1;
				}

			} while( list_next( generic_load_attribute_list ) );
		} while( list_next( generic_load_folder_list ) );
	}

	fclose( input_file );

}

char *generic_load_get_heading_string(	LIST *generic_load_attribute_list )
{
	static char heading_string[ 4096 ];
	char *ptr = heading_string;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( !list_rewind( generic_load_attribute_list ) )
	{
		printf( "<p>ERROR: cannot get attributes to load.\n" );
		document_close();
		exit( 0 );
	}

	do {
		generic_load_attribute =
			list_get_pointer( generic_load_attribute_list );

		if ( !generic_load_attribute->is_participating ) continue;

		if ( ptr != heading_string ) ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr,
				"%s", 
				generic_load_attribute->attribute_name );

	} while( list_next( generic_load_attribute_list ) );

	return heading_string;

}

boolean generic_load_has_participating(
				LIST *generic_load_attribute_list )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( !list_rewind( generic_load_attribute_list ) ) return 0;

	do {
		generic_load_attribute =
			list_get_pointer(
				generic_load_attribute_list );

		if ( generic_load_attribute->is_participating )
			return 1;

	} while( list_next( generic_load_attribute_list ) );

	return 0;

}

static int row_count = 0;
void generic_load_reset_row_count( void )
{
	row_count = 0;
}

boolean generic_load_skip_header_rows( int skip_header_rows )
{
	if ( skip_header_rows )
	{
		if ( row_count++ < skip_header_rows )
		{
			return 1;
		}
	}
	return 0;
}

boolean generic_load_delimiters_only(
				char *input_buffer,
				char delimiter )
{
	while( *input_buffer )
	{
		if ( *input_buffer != delimiter ) return 0;
		input_buffer++;
	}

	return 1;
}

char *generic_load_get_primary_key_list_string(
				LIST *generic_load_attribute_list )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	static char primary_key_list_string[ 1024 ];
	char *string_ptr = primary_key_list_string;

	if ( !list_rewind( generic_load_attribute_list ) ) return (char *)0;

	do {
		generic_load_attribute =
			list_get_pointer(
				generic_load_attribute_list );

		if ( !generic_load_attribute->is_participating
		&&   generic_load_attribute->is_primary_key )
		{
			return (char *)0;
		}

		if ( generic_load_attribute->is_primary_key )
		{
			if ( string_ptr != primary_key_list_string )
			{
				string_ptr += sprintf( string_ptr, "," );
			}

			string_ptr += sprintf(
					string_ptr,
					"%s",
					generic_load_attribute->
						attribute_name );
		}

	} while( list_next( generic_load_attribute_list ) );

	return primary_key_list_string;

}

boolean generic_load_build_sys_string(
			char *sys_string,
			char *application_name,
			char *folder_name,
			LIST *generic_load_attribute_list,
			boolean sort_unique )
{
	char *table_name;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	char *stderr_string;
	int first_time = 1;
	boolean did_any = 0;
	char *sort_unique_process;

	if ( !list_rewind( generic_load_attribute_list ) ) return 0;


	if ( sort_unique )
	{
		sort_unique_process = "sort -u";
		stderr_string =
		"2>&1 | grep -vi duplicate | html_paragraph_wrapper.e";
	}
	else
	{
		sort_unique_process = "cat";
		stderr_string = "2>&1 | html_paragraph_wrapper.e";
	}

	table_name = get_table_name( application_name, folder_name );

	sys_string += sprintf(	sys_string,
				"%s | insert_statement.e table=%s field='",
				sort_unique_process,
				table_name );

	do {
		generic_load_attribute =
			list_get_pointer(
				generic_load_attribute_list );

		if ( !generic_load_attribute->is_participating ) continue;

		if ( first_time )
			first_time = 0;
		else
			sys_string += sprintf( sys_string, "," );

		sys_string += sprintf(	sys_string,
					"%s",
					generic_load_attribute->
						attribute_name );
	} while( list_next( generic_load_attribute_list ) );
	did_any = 1;

#ifdef DEBUG_MODE
{
	sprintf(sys_string,
"' delimiter='%c' | cat %s | html_paragraph_wrapper",
		INSERT_DELIMITER,
		stderr_string );
}
#else
{
	sprintf(sys_string,
		"' delimiter='%c' | sql.e %s",
		INSERT_DELIMITER,
		stderr_string );
}
#endif

	return did_any;

}

int generic_load_output_test_only_report(
			char *load_filename,
			char delimiter,
			LIST *generic_load_attribute_list,
			int primary_key_date_offset,
			int primary_key_time_offset,
			char *application_name,
			boolean display_errors,
			char *folder_name,
			int skip_header_rows_integer,
			boolean with_sort_unique )
{
	char input_buffer[ 4096 ];
	char *piece_buffer;
	char buffer[ 128 ];
	unsigned int input_record_count = 0;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	FILE *input_file;
	FILE *error_file;
	char error_filename[ 128 ];
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading_string;
	char *sort_process;

	if ( !load_filename
	||   !( input_file = fopen( load_filename, "r" ) ) )
	{
		printf( "<p>ERROR: please transmit a file.\n" );
		document_close();
		exit( 0 );
	}

	/* Open the error file */
	/* ------------------- */
	sprintf(error_filename,
		"/tmp/generic_load_error_file_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	heading_string =
		generic_load_get_heading_string(
			generic_load_attribute_list );

	if ( with_sort_unique )
		sort_process = "sort -u";
	else
		sort_process = "cat";

	sprintf( sys_string,
"%s | queue_top_bottom_lines.e %d | html_table.e '^^%s' '%s' '^'",
		 sort_process,
		 TEST_RECORD_OUTPUT_COUNT,
		 format_initial_capital( buffer, folder_name ),
		 heading_string );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( generic_load_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty generic_load_attribute_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_reset_row_count();

	while( get_line( input_buffer, input_file ) )
	{
		trim( input_buffer );

		if ( generic_load_skip_header_rows(
			skip_header_rows_integer ) )
			continue;

		if ( !*input_buffer ) continue;
		if ( *input_buffer == '#' ) continue;

		if ( generic_load_delimiters_only(
				input_buffer,
				delimiter ) )
		{
			continue;
		}

		input_record_count++;

		generic_load_set_international_date(
				input_buffer,
				application_name,
				delimiter,
				primary_key_date_offset );

		generic_load_replace_time_2400_with_0000(
				input_buffer,
				delimiter,
				primary_key_date_offset,
				primary_key_time_offset );

		if ( !generic_load_fix_time(
				input_buffer,
				delimiter,
				primary_key_time_offset ) )
		{
			fprintf( error_file,
			 	"Warning: bad time in %s\n",
			 	input_buffer );
			continue;
		}

		list_rewind( generic_load_attribute_list );
		do {
			generic_load_attribute =
				list_get_pointer(
					generic_load_attribute_list );

			if ( !generic_load_attribute->is_participating )
				continue;

			piece_buffer =
				generic_load_get_piece_buffer(
					input_buffer,
					&delimiter,
					application_name,
					generic_load_attribute->datatype,
					generic_load_attribute->
						constant,
					generic_load_attribute->
						position - 1,
					generic_load_attribute->
						is_primary_key );

			if ( !list_at_head( generic_load_attribute_list ) )
				fprintf( output_pipe, "^" );

			if ( *piece_buffer )
			{
				fprintf(	output_pipe,
						"%s",
						piece_buffer );
			}

		} while( list_next( generic_load_attribute_list ) );
		fprintf( output_pipe, "\n" );
	}

	fclose( input_file );
	fclose( error_file );
	pclose( output_pipe );

	if ( display_errors ) timlib_display_error_file( error_filename );

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};

	return input_record_count;

}

int generic_load_output_to_database(
			char *application_name,
			char *load_filename,
			char delimiter,
			LIST *generic_load_folder_list,
			int skip_header_rows_integer )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	int input_record_count = 0;
	char input_buffer[ 4096 ];
	char *piece_buffer;
	GENERIC_LOAD_FOLDER *generic_load_folder;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	int first_time;
	FILE *input_file;
	FILE *error_file;
	char error_filename[ 128 ];

	if ( !list_rewind( generic_load_folder_list ) ) return 0;

	/* Open the error file */
	/* ------------------- */
	sprintf(error_filename,
		"/tmp/generic_load_error_file_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		exit( 1 );
	}

	do {
		generic_load_folder =
			list_get_pointer( generic_load_folder_list );

		if ( !generic_load_build_sys_string(
				sys_string,
				application_name,
				generic_load_folder->folder_name,
				generic_load_folder->
					generic_load_attribute_list,
				!list_at_first( generic_load_folder_list )
				/* sort_unique */ ) )
		{
			continue;
		}

		if ( !load_filename
		||   !( input_file = fopen( load_filename, "r" ) ) )
		{
			return 0;
		}

		output_pipe = popen( sys_string, "w" );

		fflush( stdout );
		first_time = 1;

		generic_load_reset_row_count();

		while( get_line( input_buffer, input_file ) )
		{
			trim( input_buffer );

			if ( generic_load_skip_header_rows(
				skip_header_rows_integer ) )
			{
				continue;
			}

			if ( !*input_buffer ) continue;
			if ( *input_buffer == '#' ) continue;

			if ( generic_load_delimiters_only(
				input_buffer, delimiter ) )
			{
				continue;
			}

			generic_load_set_international_date(
				input_buffer,
				application_name,
				delimiter,
				generic_load_folder->primary_key_date_offset );

			generic_load_replace_time_2400_with_0000(
				input_buffer,
				delimiter,
				generic_load_folder->primary_key_date_offset,
				generic_load_folder->primary_key_time_offset );

			if ( !generic_load_fix_time(
				input_buffer,
				delimiter,
				generic_load_folder->
					primary_key_time_offset ) )
			{
				fprintf( error_file,
			 		"Warning: bad time in %s\n",
			 		input_buffer );
				continue;
			}

			if ( list_at_first( generic_load_folder_list ) )
				input_record_count++;

			if ( !list_rewind( generic_load_folder->
						generic_load_attribute_list ) )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: empty generic_load_attribute_list.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			first_time = 1;
			do {
				generic_load_attribute =
					list_get_pointer(
						generic_load_folder->
						generic_load_attribute_list );

				if ( !generic_load_attribute->is_participating )
					continue;

				piece_buffer =
					generic_load_get_piece_buffer(
						input_buffer,
						&delimiter,
						application_name,
						generic_load_attribute->
							datatype,
						generic_load_attribute->
							constant,
						generic_load_attribute->
							position - 1,
						generic_load_attribute->
							is_primary_key );

				if ( first_time
				&&   *piece_buffer )
				{
					first_time = 0;
				}
				else
				{
					fprintf(output_pipe,
						"%c",
						INSERT_DELIMITER );
				}

				if ( *piece_buffer )
				{
					fprintf(output_pipe,
						"%s",
						piece_buffer );
				}
			} while ( list_next(
					generic_load_folder->
						generic_load_attribute_list ) );
			fprintf( output_pipe, "\n" );
		}
		pclose( output_pipe );
		fclose( input_file );
	} while( list_next( generic_load_folder_list ) );

	printf( "<p>Process complete -- Input count: %d\n",
		input_record_count );

	fclose( error_file );
	timlib_display_error_file( error_filename );

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};

	return input_record_count;
}

int generic_load_get_position_integer(	DICTIONARY *position_dictionary,
					char *attribute_name,
					char *related_attribute_name )
{
	char *position_string;
	int position_integer = 0;

	position_string =
		dictionary_get_pointer(
			position_dictionary,
			attribute_name );

	if ( !position_string
	||   !*position_string
	||   ! ( position_integer =
			atoi( position_string ) ) )
	{
		if ( related_attribute_name )
		{
			position_string =
				dictionary_get_pointer(
					position_dictionary,
					related_attribute_name );

			if ( position_string )
			{
				position_integer = atoi( position_string );
			}
		}
	}
	return position_integer;
}

void generic_load_set_international_date(
				char *input_buffer,
				char *application_name,
				char delimiter,
				int primary_key_date_offset )
{
	char date_time_string[ 1024 ];
	char date_string[ 16 ];
	char time_string[ 16 ];
	boolean make_substitution = 0;

	if ( primary_key_date_offset == -1 ) return;

	if ( !piece(	date_time_string,
			delimiter,
			input_buffer,
			primary_key_date_offset ) )
	{
		return;
	}


	if ( character_exists( date_time_string, ' ' ) )
	{
		piece( date_string, ' ', date_time_string, 0 );
		piece( time_string, ' ', date_time_string, 1 );
	}
	else
	{
		timlib_strcpy( date_string, date_time_string, 16 );
		*time_string = '\0';
	}

	if ( date_convert_date_get_format( date_string ) != international )
	{
		DATE_CONVERT *date_convert;

		date_convert =
			date_convert_new_database_format_date_convert(
				application_name,
				date_string );
	
		if ( date_convert )
		{
			strcpy( date_string, date_convert->return_date );

			date_convert_free( date_convert );
			make_substitution = 1;
		}
	}

	/* If "2017-03-30 10:45" */
	/* --------------------- */
	if ( *time_string )
	{
		if ( character_exists( time_string, ':' ) )
		{
			date_remove_colon_in_time( time_string );
			make_substitution = 1;
		}

		sprintf(	date_time_string,
				"%s%c%s",
				date_string,
				delimiter,
				time_string );
	}
	else
	{
		strcpy( date_time_string, date_string );
	}

	if ( make_substitution )
	{
		piece_replace(	input_buffer,
				delimiter, 
				date_time_string,
				primary_key_date_offset );
	}

}

GENERIC_LOAD_CHOOSE *generic_load_choose_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *process_name,
			char *role_name,
			boolean menu_boolean )
{
	GENERIC_LOAD_CHOOSE *generic_load_choose;

	if ( !application_name
	||   !login_name
	||   !session_key
	||   !process_name
	||   !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_choose = generic_load_choose_calloc();

	generic_load_choose->role =
		role_fetch(
			role_name,
			0 /* not fetch_attribute_exclude_list */ );

	if ( menu_boolean )
	{
		generic_load_choose->folder_menu =
			folder_menu_fetch(
				application_name,
				session_key,
				appaserver_parameter_data_directory(),
				role_name );

		if ( !generic_load_choose->folder_menu )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		generic_load_choose->menu =
			menu_fetch(
				application_name,
				login_name,
				session_key,
				role_name,
				FRAMESET_PROMPT_FRAME,
				folder_menu->lookup_count_list );

		if ( !generic_load_choose->menu )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	} /* if menu_boolean */

	generic_load_choose->post_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_choose_post_action_string(
			application_name,
			login_name,
			session_key,
			role_name );

	generic_load_choose->prompt_html =
		/* -------------------- */
		/* Return static memory */
		/* -------------------- */
		generic_load_choose_prompt_html(
			process_name );

	generic_load_choose->title_string =
		/* -------------------- */
		/* Return static memory */
		/* -------------------- */
		generic_load_choose_title_string(
			process_name );

	generic_load_choose->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			generic_load_choose->title_string,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			menu_boolean,
			menu,
			document_head_menu_setup_string( 
				menu_boolean ),
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	generic_load_choose->form_generic_load_choose =
		form_generic_load_choose_new(
			generic_load_choose->prompt_html,
			role_folder_name_list(
				role_folder_insert_list(
					role_name ) ),
			generic_load_choose->post_action_string );


	if ( !generic_load_choose->form_generic_load_choose )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: form_generic_load_choose_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_choose->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_choose_html(
			generic_load_choose->document->html,
			generic_load_choose->document->document_head->html,
			document_head_close_html(),
			generic_load_choose->document->document_body->html,
			generic_load_choose->form_generic_load_choose->html,
			document_body_close_html(),
			document_close_html() );

	return generic_load_choose;
}

char *generic_load_choose_post_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name )
{
	char post_action_string[ 1024 ];

	if ( !application_name
	||   !login_name
	||   !session_key
	||   !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(post_action_string,
		" action=\"%s/%s?%s+%s+%s+%s\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		"generic_load_choose_post",
		application_name,
		login_name,
		session_key,
		role_name );

	return strdup( post_action_string );
}

char *generic_load_choose_prompt_html(
			char *process_name )
{
	static char prompt_html[ 128 ];
	char buffer[ 64 ];

	sprintf(prompt_html,
		"<h3>%s choose a table:</h3>",
		string_initial_capital(
			buffer,
			process_name ) );

	return prompt_html;
}

char *generic_load_choose_title_string(
			char *process_name )
{
	static char title_string[ 128 ];

	string_initial_capital(
		title_string,
		process_name ) );

	return title_string;
}

char *generic_load_choose_html(
			char *document_html,
			char *document_head_html,
			char *document_body_html,
			char *form_generic_load_html,
			char *document_body_close_html,
			char *document_close_html )
{
	char html[ STRING_64K ];

	if ( !document_html,
	||   !document_head_html,
	||   !document_body_html,
	||   !form_generic_load_html,
	||   !document_body_close_html,
	||   !document_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s\n%s\n%s",
		document_html,
		document_head_html,
		document_body_html,
		form_generic_load_html,
		document_body_close_html,
		document_close_html );

	return strdup( html );
}

GENERIC_LOAD_CHOOSE *generic_load_choose_calloc( void )
{
	GENERIC_LOAD_CHOOSE *generic_load_choose;

	if ( ! ( generic_load_choose =
			calloc( 1, sizeof( GENERIC_LOAD_CHOSE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_choose;
}

GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form_calloc( void )
{
	GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form;

	if ( ! ( generic_load_choose_form =
			calloc( 1, sizeof( GENERIC_LOAD_CHOOSE_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_choose_form;
}

GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form_new(
			char *prompt_html,
			char *drop_down_name,
			LIST *role_folder_insert_name_list,
			char *post_action_string )
{
	GENERIC_LOAD_CHOOSE_FORM *generic_load_choose_form =
		generic_load_choose_form_calloc();

	generic_load_choose_form->tag_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag_html(
			"generic_load_choose" /* form_name */,
			post_action_string,
			FRAMESET_PROMPT_FRAME /* target_frame */ );

	generic_load_choose_form->element_list =
		generic_load_choose_form_element_list(
			role_folder_insert_name_list,
			drop_down_name );

	generic_load_choose_form->html =
		generic_load_choose_form_html(
			generic_load_choose_form->tag_html,
			generic_load_choose_form->element_list,
			form_close_html() );

	return generic_load_choose_form;
}

LIST *generic_load_choose_form_element_list(
			LIST *role_folder_insert_name_list,
			char *drop_down_name )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	/* Create a table */
	/* -------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	/* Create a table row */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	/* Create a drop-down */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				drop_down,
				(char *)0 /* element_name */ ) ) );

	free( element->drop_down );

	element->drop_down =
		element_drop_down_new(
			drop_down_name,
			(LIST *)0 /* attribute_name_list */,
			role_folder_insert_name_list /* delimited_list */,
			(LIST *)0 /* display_list */,
			0 /* not no_initial_capital */,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			0 /* not output_select_option */,
			element_drop_down_display_size(
				list_length(
					role_folder_insert_name_list ) ),
			-1 /* tab order */,
			0 /* not multi_select */,
			(char *)0 /* post_change_javascript */,
			0 /* not readonly */,
			0 /* not recall */ );

	/* Close the table */
	/* --------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

char *generic_load_choose_form_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	form_element_list_html(
		tag_html,
		element_list,
		form_close_html );
}

GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			DICTIONARY *working_post_dictionary )
{
	GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post =
		generic_load_choose_post_calloc();

	generic_load_choose_post->folder_name =
	/* --------------------------------------------------------------- */
	/* Returns working_post_dictionary->hash_table->other_data or null */
	/* --------------------------------------------------------------- */
		generic_load_choose_post_folder_name(
			GENERIC_LOAD_CHOOSE_POST_DROP_DOWN_NAME,
			working_post_dictionary );

	if ( !generic_load_choose_post->folder_name )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: generic_load_choose_post_folder_name() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_choose_post->system_string =
		generic_load_choose_post-system_string(
			application_name,
			session_key,
			login_name,
			role_name,
			generic_load_choose_post->folder_name );

	return generic_load_choose_post;
}

char *generic_load_choose_post_folder_name,
			char *drop_down_name,
			DICTIONARY *working_post_dictionary )
{
	return
	dictionary_get(
		drop_down_name,
		working_post_dictionary );
}

char *generic_load_choose_post_system_string(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *post_folder_name )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !post_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"generic_load_folder %s %s %s %s %s",
		application_name,
		session_key,
		login_name,
		role_name,
		post_folder_name );

	return strdup( system_string );
}

GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post_calloc( void )
{
	GENERIC_LOAD_CHOOSE_POST *generic_load_choose_post;

	if ( ! ( generic_load_choose_post =
			calloc( 1, sizeof( GENERIC_LOAD_CHOOSE_POST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_choose_post;
}

GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form_new(
			char *prompt_html,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *post_action_string,
			char *login_name )
{
	GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form =
		generic_load_folder_form_calloc();

	generic_load_folder_form->tag_html =
		form_tag_html(
			“generic_load_folder” /* form_name */,
			post_action_string,
			FRAMESET_EDIT_FRAME /* target_frame */ );

	generic_load_folder_form->generic_load_folder_element_list =
		generic_load_folder_element_list_new(
			prompt_html,
			folder_attribute_list,
			relation_mto1_non_isa_list,
			GENERIC_LOAD_UPLOAD_LABEL,
			GENERIC_LOAD_SKIP_HEADER_ROWS,
			login_name );

	generic_load_folder_form->html =
		generic_load_folder_form_html(
			generic_load_folder_form->tag_html,
			generic_load_folder_element_list->html,
			form_close_html() );

	return generic_load_folder_form;
}

char *generic_load_folder_form_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	form_element_list_html(
		tag_html,
		element_list,
		form_close_html );
}

GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form_calloc(
			void )
{
	GENERIC_LOAD_FOLDER_FORM *generic_load_folder_form;

	if ( ! ( generic_load_folder_form =
			calloc( 1, sizeof( GENERIC_LOAD_FOLDER_FORM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_folder_form;
}

GENERIC_LOAD_FOLDER_ELEMENT_LIST *
	generic_load_folder_element_list_new(
			char *generic_load_folder_prompt_html,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_listd,
			char *generic_load_upload_label,
			char *generic_load_skip_header_rows,
			char *login_name )
{
	ROW_SECURITY_RELATION *row_security_relation;
	FOLDER_ATTRIBUTE *folder_attribute;
	int default_position = 1;
	int primary_key_default_position;
	char buffer[ 256 ];
	char post_change_javascript[ 128 ];
	APPASERVER_ELEMENT *element;
	LIST *row_security_relation_list = {0};
	LIST *element_list = list_new();

	if ( !list_length( folder_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set_list(
		element_list,
		generic_load_buttons_element_list() );

	list_set_list(
		element_list,
		generic_load_upload_element_list(
			generic_load_upload_label ) );

	list_set_list(
		element_list,
		generic_load_skip_header_rows_element_list(
			generic_load_skip_header_rows ) );

	list_set_list(
		element_list,
		generic_load_execute_checkbox_element_list() );

	list_set_list(
		element_list,
		generic_load_dialog_box_element_list() );

	list_rewind( folder_attribute_list );

	do {
		folder_attribute = list_get( folder_attribute_list );

		if ( ( row_security_relation =
			row_security_relation_new(
				folder_attribute->attribute_name,
				relation_mto1_non_isa_list,
				(char *)0 /* post_change_javascript */,
				(DICTIONARY *)0 /* drillthru_dictionary */,
				login_name,
				(char *)0 /* security_entity_where */,
				0 /* not viewonly */,
				row_security_relation_list ) ) )
		{
			if ( list_length(
				row_security_relation->
					relation->
					foreign_key_list ) > 1 )
			{
				goto skip_relation;
			}

			if ( !row_security_relation_list )
			{
				row_security_relation_list = list_new();
			}

			list_set(
				row_security_relation_list,
				row_security_relation);

			list_set(
				element_list,
				appaserver_element_new(
					table_row,
					(char *)0 ) );

			list_set(
				element_list,
				appaserver_element_new(
					table_data,
					(char *)0 ) );

			list_set_list(
				element_list,
				row_security_relation->element_list );
		}

		if ( attribute->primary_key_index )
		{
			printf( "*" );
			primary_key_default_position = default_position;
		}
		else
		{
			primary_key_default_position = 0;
		}

		printf( "%s",
			format_initial_capital( buffer,
						attribute->attribute_name ) );
		if ( attribute->hint_message
		&&   *attribute->hint_message )
		{
			printf( "(%s)", attribute->hint_message );
		}

		printf( "</td>\n" );
		printf(
	"<td><input name=%s%s type=text size=3 value=%d\n"
	" onChange=\"after_field('%s%s','%s%s',%d)\"></td>\n",
			POSITION_PREFIX,
			attribute->attribute_name,
			default_position,
			POSITION_PREFIX,
			attribute->attribute_name,
			CONSTANT_PREFIX,
			attribute->attribute_name,
			primary_key_default_position );

		/* If drop-down element */
		/* -------------------- */
		if ( element )
		{
			sprintf(post_change_javascript,
				"after_field('%s%s','%s%s',%d)",
				POSITION_PREFIX,
				attribute->attribute_name,
				CONSTANT_PREFIX,
				attribute->attribute_name,
				primary_key_default_position );

			element_drop_down_output(
				stdout,
				element->name,
				element->drop_down->option_data_list,
				element->drop_down->option_label_list,
				element->drop_down->number_columns,
				element->drop_down->multi_select,
				-1 /* row */,
				element->drop_down->initial_data,
				element->drop_down->output_null_option,
				element->drop_down->output_not_null_option,
				element->drop_down->output_select_option,
				post_change_javascript,
				element->drop_down->max_drop_down_size,
				element->drop_down->multi_select_element_name,
				element->drop_down->onblur_javascript_function,
				(char *)0 /* background_color */,
				element->drop_down->date_piece_offset,
				element->drop_down->no_initial_capital,
				element->drop_down->readonly,
				0 /* tab_index */,
				element->drop_down->state );
			fflush( stdout );
		}
		else
		/* ------------------ */
		/* Else field element */
		/* ------------------ */
		{
			printf(
	"<td><input name=%s%s type=text size=20 maxlength=%d"
	" onChange=\"after_field('%s%s','%s%s',%d)",
				CONSTANT_PREFIX,
				attribute->attribute_name,
				attribute->width,
				POSITION_PREFIX,
				attribute->attribute_name,
				CONSTANT_PREFIX,
				attribute->attribute_name,
				primary_key_default_position );

			if ( strcmp( attribute->datatype, "time" ) == 0)
				printf( " && validate_time_insert(this)" );
			printf( "\"></td>\n" );
		}

		if ( !attribute->primary_key_index )
		{
			printf(
	"<td><input name=ignore_%s type=checkbox value=yes"
	" onChange=\"after_ignore(this, '%s%s','%s%s')\">"
	"Ignore</td>\n",
				attribute->attribute_name,
				POSITION_PREFIX,
				attribute->attribute_name,
				CONSTANT_PREFIX,
				attribute->attribute_name );
		}

		default_position++;

	} while( list_next( attribute_list ) );

	return element_list;
}

LIST *generic_load_buttons_element_list( void )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	/* Create <Submit> */
	/* --------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button =
		button_submit(
			(char *)0 /* multi_select_all_javascript */,
			(char *)0 /* keystrokes_save_javascript */,
			(char *)0 /* keystrokes_multi_save_javascript */,
			(char *)0 /* verify_attribute_widths_javascript */,
			0 /* form_number */ );

	/* Create <Reset> */
	/* -------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button =
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ );

	list_set(
		element_list,
		( element = appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

LIST *generic_load_upload_element_list(
			char *generic_load_upload_label )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	/* Create the filename prompt */
	/* -------------------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	free( element->non_edit_text );

	element->non_edit_text =
		element_non_edit_text_new(
			(char *)0 /* attribute_name */,
			"Filename" /* message */ );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				upload,
				(char *)0 /* element_name */ ) ) );

	free( element->upload );

	element->upload =
		element_upload_new(
			generic_load_upload_label /* attribute_name */,
			-1 /* tab_order */,
			1 /* recall */ );

	element->upload->html =
		element_upload_insert_html(
			generic_load_upload_label /* element_name */,
			-1 /* tab_order */ );

	return element_list;
}

LIST *generic_load_skip_header_rows_element_list(
			char *generic_load_upload_label )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	/* Create skip header rows */
	/* ----------------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	element->non_edit_text->message = "Skip Header Rows";

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				text,
				(char *)0 /* element_name */ ) ) );

	element->text->attribute_name = generic_load_skip_header_rows;
	element->text->attribute_width_max_length = 3;

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	element->non_edit_text->message = "How many header rows to skip?";

	return element_list;
}

LIST *generic_load_execute_checkbox_element_list( void )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );


	list_set(
		element_list,
		( element =
			appaserver_element_new(
				checkbox,
				(char *)0 /* element_name */ ) ) );

	element->checkbox->prompt_string = "Execute?";
	element->checkbox->element_name = "execute_yn";

	list_set(
		element_list,
		( element = appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

LIST *generic_load_dialog_box_element_list( void )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	list_set(
		element_list,
		( element = appaserver_element_new(
				line_break,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_heading,
				(char *)0 /* element_name */ ) ) );

	element->table_heading->heading_string = "Attribute";

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_heading,
				(char *)0 /* element_name */ ) ) );

	element->table_heading->heading_string = "Position";

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_heading,
				(char *)0 /* element_name */ ) ) );

	element->table_heading->heading_string = "Constant";

	return element_list;
}
