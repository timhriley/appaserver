/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/appaserver_library.c			*/
/* --------------------------------------------------------------------	*/
/*									*/
/* These are the generic Appaserver functions.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "application.h"
#include "attribute.h"
#include "element.h"
#include "appaserver.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "operation.h"
#include "date.h"
#include "column.h"
#include "environ.h"
#include "date_convert.h"
#include "appaserver_error.h"
#include "basename.h"
#include "element.h"
#include "appaserver_library.h"

static char *system_folder_list[] = {
					"additional_user_drop_down_attribute",
					"additional_drop_down_attribute",
					"appaserver_sessions",
					"appaserver_user",
					"application",
					"application_constants",
					"attribute",
					"attribute_datatype",
					"attribute_exclude",
					"date_formats",
					"drop_down_prompt",
					"drop_down_prompt_data",
					"folder",
					"folder_attribute",
					"folder_row_level_restrictions",
					"foreign_attribute",
					"form",
					"grace_output",
					"login_default_role",
					"operation",
					"permissions",
					"process",
					"process_generic_datatype_folder",
					"process_generic_value_folder",
					"process_generic_output",
					"process_generic_units",
					"process_groups",
					"process_set",
					"process_set_parameter",
					"process_parameter",
					"prompt",
					"relation",
					"role",
					"role_folder",
					"role_operation",
					"role_appaserver_user",
					"role_process",
					"role_process_set_member",
					"row_level_restrictions",
					"row_security_role_update",
					"select_statement",
					"subschemas",
					"upgrade_scripts",
					(char *)0 };

void output_update_pipe_string( 	FILE *destination,
					char *attribute_name,
					char *data,
					LIST *primary_key_data_list )
{
	fprintf( destination,
		 "%s|",
		 list_display( primary_key_data_list ) );
	fprintf( destination,
		 "%s=%s",
		 attribute_name,
		 data );
	fprintf( destination, "\n" );
}

char *appaserver_library_full_attribute_name(
			char *application_name,
			char *folder_name,
			char *attribute_name )
{
	static char full_attribute_name[ 512 ];

	if ( folder_name && *folder_name )
	{
		sprintf(full_attribute_name,
			"%s.%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_library_table_name(
				application_name,
				folder_name ),
			attribute_name );
	}
	else
	{
		strcpy( full_attribute_name, attribute_name );
	}

	return full_attribute_name;
}

char *get_multiple_table_names(
			char *application_name,
			char *folder_name_list_comma_string )
{
	return get_multi_table_name(
			application_name,
			folder_name_list_comma_string );
}

char *get_multi_table_name(
			char *application_name,
			char *multi_folder_name_list_string )
{
	char multi_table_name[ 2048 ];
	char *ptr = multi_table_name;
	char folder_name[ 128 ];
	char *table_name;
	int i;

	for(	i = 0;
		piece(	folder_name,
			',',
			multi_folder_name_list_string,
			i );
		i++ )
	{
		if ( i ) ptr += sprintf( ptr, "," );

		table_name =
			/* ---------------------- */
			/* Returns static memory */
			/* ---------------------- */
			appaserver_library_table_name(
				application_name,
				folder_name );

		ptr += sprintf( ptr, "%s", table_name );
	}

	return strdup( multi_table_name );
}

char *get_table_name(
			char *application_name,
			char *folder_name )
{
	return strdup(
			appaserver_library_table_name(
				application_name,
				folder_name ) );
}

char *appaserver_library_table_name(
			char *application_name,
			char *folder_name )
{
	static char table_name[ 512 ];

	*table_name = '\0';

	if ( !folder_name || !*folder_name )
	{
		return table_name;
	}

	if ( application_name
	&&   string_strcmp( folder_name, "application" ) == 0 )
	{
		sprintf( table_name, "%s_%s", application_name, folder_name );
	}
	else
	{
		strcpy( table_name, folder_name );
	}

	return table_name;
}

/* Sample: dictionary_string = "first_name=tim&last_name=riley" */
/* ------------------------------------------------------------ */
void output_dictionary_string_as_hidden( char *dictionary_string )
{
	char pair_string[ 4096 ];
	char attribute[ 2048 ];
	char data[ 2048 ];
	int i;

	for( i = 0; piece( pair_string, '&', dictionary_string, i ); i++ )
	{
		piece( attribute, '=', pair_string, 0 );
		if ( piece( data, '=', pair_string, 1 ) )
		{
			printf( 
			"<input type=\"hidden\"	name=\"%s\" value=\"%s\">\n",
			attribute, data );
		}
	}
}

void appaserver_library_output_dictionary_as_hidden( DICTIONARY *dictionary )
{
	output_dictionary_as_hidden( dictionary );
}

void output_dictionary_as_hidden( DICTIONARY *dictionary )
{
	LIST *key_list = dictionary_key_list( dictionary );
	char *key;
	char *data;

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get( key_list );

			if ( ( data =
				dictionary_get(
					key,
					dictionary ) ) )
			{
				printf( 
			"<input name=\"%s\" type=\"hidden\" value=\"%s\">\n",
					key,
					data );
			}
		} while( list_next( key_list ) );
	}
	list_free_container( key_list );
}

LIST *get_relation_operator_list( char *datatype )
{
	LIST *list;

	list = list_new();

	if ( strcmp( datatype, "date" ) == 0
	||   strcmp( datatype, "time" ) == 0 )
	{
		list_append_string( list, BEGINS_OPERATOR );
		list_append_string( list, EQUAL_OPERATOR );
		list_append_string( list, BETWEEN_OPERATOR );
		list_append_string( list, OR_OPERATOR );
		list_append_string( list, GREATER_THAN_OPERATOR );
		list_append_string( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, LESS_THAN_OPERATOR );
		list_append_string( list, LESS_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, NOT_EQUAL_OPERATOR );
		list_append_string( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}
	else
	if ( strcmp( datatype, "notepad" ) == 0 )
	{
		list_append_string( list, CONTAINS_OPERATOR );
		list_append_string( list, NOT_CONTAINS_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}
	else
	if ( strcmp( datatype, "text" ) == 0
	||   strcmp( datatype, "http_filename" ) == 0 )
	{
		list_append_string( list, BEGINS_OPERATOR );
		list_append_string( list, EQUAL_OPERATOR );
		list_append_string( list, CONTAINS_OPERATOR );
		list_append_string( list, OR_OPERATOR );
		list_append_string( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, NOT_CONTAINS_OPERATOR );
		list_append_string( list, NOT_EQUAL_OPERATOR );
		list_append_string( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}
	else
	{
		list_append_string( list, EQUAL_OPERATOR );
		list_append_string( list, BETWEEN_OPERATOR );
		list_append_string( list, BEGINS_OPERATOR );
		list_append_string( list, CONTAINS_OPERATOR );
		list_append_string( list, NOT_CONTAINS_OPERATOR );
		list_append_string( list, OR_OPERATOR );
		list_append_string( list, NOT_EQUAL_OPERATOR );
		list_append_string( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_append_string( list, GREATER_THAN_OPERATOR );
		list_append_string( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, LESS_THAN_OPERATOR );
		list_append_string( list, LESS_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}

	return list;
}

LIST *get_relation_operator_equal_list()
{
	static LIST *list = {0};

	if ( list ) return list;

	list = list_new();
	list_append_string( list, EQUAL_OPERATOR );
	return list;
}

char *appaserver_library_operator_character(
			char *operator_string )
{
	if ( strcmp( operator_string, EQUAL_OPERATOR ) == 0 )
		return "=";
	else
	if ( strcmp( operator_string, NOT_EQUAL_OPERATOR ) == 0 )
		return "<>";
	else
	if ( strcmp( operator_string, GREATER_THAN_OPERATOR ) == 0 )
		return ">";
	else
	if ( strcmp( operator_string, GREATER_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return ">=";
	else
	if ( strcmp( operator_string, LESS_THAN_OPERATOR ) == 0 )
		return "<";
	else
	if ( strcmp( operator_string, LESS_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return "<=";
	else
	if ( strcmp( operator_string, BEGINS_OPERATOR ) == 0 )
		return BEGINS_OPERATOR;
	else
	if ( strcmp( operator_string, CONTAINS_OPERATOR ) == 0 )
		return CONTAINS_OPERATOR;
	else
	if ( strcmp( operator_string, OR_OPERATOR ) == 0 )
		return OR_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_CONTAINS_OPERATOR ) == 0 )
		return NOT_CONTAINS_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_EQUAL_OR_NULL_OPERATOR ) == 0 )
		return NOT_EQUAL_OR_NULL_OPERATOR;
	else
	if ( strcmp( operator_string, BETWEEN_OPERATOR ) == 0 )
		return BETWEEN_OPERATOR;
	else
	if ( strcmp( operator_string, NULL_OPERATOR ) == 0 )
		return NULL_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_NULL_OPERATOR ) == 0 )
		return NOT_NULL_OPERATOR;
	else
		return "unknown operator";
}

boolean appaserver_library_is_system_folder( char *folder_name )
{
	return is_system_folder( folder_name );
}

boolean is_system_folder( char *folder_name )
{
	char **f;

	for ( f = system_folder_list; *f; f++ )
		if ( strcmp( folder_name, *f ) == 0 )
			return 1;
	return 0;
}

char **get_system_folder_list( void )
{
	return system_folder_list;
}

char **appaserver_library_system_folder_list( void )
{
	return system_folder_list;
}

void appaserver_library_set_no_display_pressed(
					DICTIONARY *ignore_dictionary, 
					LIST *key_list )
{
	char *key;
	char no_display_key[ 1024 ];

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get( key_list );

			sprintf( no_display_key,
				 "%s_0",
				 key );

			dictionary_set_pointer(
				ignore_dictionary,
				strdup( no_display_key ),
				"yes" );
		} while( list_next( key_list ) );
	}
}

LIST *appaserver_library_no_display_pressed_attribute_name_list( 	
			DICTIONARY *ignore_dictionary,
			LIST *attribute_name_list )
{
	LIST *return_list = list_new_list();
	char *attribute_name;
	char key_to_search[ 128 ];

	if ( !list_reset( attribute_name_list ) ) return (LIST *)0;

	if ( !ignore_dictionary ) return return_list;

	do {
		attribute_name =
			list_get( attribute_name_list );

		sprintf( key_to_search,
			 "%s_0",
			 attribute_name );

		if ( dictionary_key_exists(
			ignore_dictionary,
			key_to_search ) )
		{
			list_append_unique_string(
						return_list, 
						attribute_name );
		}

	} while( list_next( attribute_name_list ) );

	return return_list;
}

int get_attribute_width(	char *application,
				char *attribute_name )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "width4attribute.sh %s %s 2>>%s",
		 application,
		 attribute_name,
		 appaserver_library_error_filename( application ) );

	results = pipe2string( sys_string );
	if ( !results )
		return 0;
	else
		return atoi( results );
}


LIST *get_date_slot_list( 	char *begin_date,
				char *end_date )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_date_slot_list %s %s",
		 begin_date,
		 end_date );
	return pipe2list( sys_string );
}

LIST *get_attribute4table_list( char *table_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, "attributes4table.sh %s", table_name );
	return pipe2list( sys_string );
}


void populate_no_display_button_for_ignore( DICTIONARY *dictionary )
{
	LIST *attribute_name_list;
	char *attribute_name;
	char *data;
	char trimmed_attribute_name[ 256 ];
	char ignored_attribute_name[ 256 ];
	int str_len = strlen( IGNORE_SELECT_PUSH_BUTTON_PREFIX );

	attribute_name_list =
		dictionary_key_list(
			dictionary );

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			attribute_name = 
				list_get( attribute_name_list );

			if ( strncmp(
				IGNORE_SELECT_PUSH_BUTTON_PREFIX,
				attribute_name,
				str_len ) == 0 )
			{
				data = dictionary_data(
						dictionary,
						attribute_name );

				strcpy(	trimmed_attribute_name,
					attribute_name + str_len );

				sprintf(ignored_attribute_name,
		 			"%s%s", 
		 			NO_DISPLAY_PUSH_BUTTON_PREFIX, 
		 			trimmed_attribute_name );

				dictionary_set_pointer(
					dictionary,
					strdup( ignored_attribute_name ),
					data );
			}
		} while( list_next( attribute_name_list ) );
	}
}

void populate_ignore_button_for_no_display_pressed( DICTIONARY *dictionary )
{
	LIST *attribute_name_list;
	char *attribute_name;
	char *data;
	char trimmed_attribute_name[ 256 ];
	char ignored_attribute_name[ 256 ];
	int str_len = strlen( NO_DISPLAY_PUSH_BUTTON_PREFIX );

	attribute_name_list =
		dictionary_key_list( dictionary );

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			attribute_name = 
				list_get( attribute_name_list );

			if ( strncmp(
				NO_DISPLAY_PUSH_BUTTON_PREFIX,
				attribute_name,
				str_len ) == 0 )
			{
				data = dictionary_data(
							dictionary,
							attribute_name );

				strcpy(	trimmed_attribute_name,
					attribute_name + str_len );

				sprintf(ignored_attribute_name,
		 			"%s%s", 
		 			IGNORE_SELECT_PUSH_BUTTON_PREFIX, 
		 			trimmed_attribute_name );

				dictionary_set_pointer(
					dictionary,
					strdup( ignored_attribute_name ),
					data );
			}
		} while( list_next( attribute_name_list ) );
	}
}

LIST *appaserver_library_datatype_name_list( char *application )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, "datatype_name_list.sh %s", application );
	return pipe2list( sys_string );
}

char *get_displayable_primary_attribute_list_string(
			LIST *primary_key_list )
{
	char buffer[ 512 ];

	return strdup( 
		format_initial_capital( buffer, 
					list_display_delimited(
						primary_key_list,
						'/' ) ) );
}

LIST *appaserver_library_attribute_name_list(
			char *application, 
			char *folder_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, 
		 "attributes4folder.sh %s %s", 
		 application, folder_name );

	return pipe2list( sys_string );
}

char *build_multi_attribute_key( LIST *key_list, char delimiter )
{
	char multi_attribute_key[ 1024 ];
	char *ptr = multi_attribute_key;
	char *key_string;
	int first_time = 1;

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key_string = list_get( key_list );

		if ( first_time )
		{
			ptr += sprintf( ptr, "%s", key_string );
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c%s", delimiter, key_string );
		}
	} while( list_next( key_list ) );

	return strdup( multi_attribute_key );
}

LIST *appaserver_library_folder_name_list(
			char *application_name,
			char *attribute_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = get_table_name( application_name, "folder_attribute" );

	sprintf(sys_string,
"echo \"select distinct folder from %s where attribute = '%s';\" | sql",
		table_name,
		attribute_name );

	return pipe2list( sys_string );
}

#ifdef NOT_DEFINED
LIST *appaserver_library_update_attribute_element_list(
			int *objects_outputted,
			ATTRIBUTE *attribute,
			char update_yn,
			LIST *primary_key_list,
			boolean is_primary_attribute,
			char *folder_post_change_javascript,
			boolean prompt_data_element_only )
{
	LIST *element_list = {0};
	LIST *return_list;
	char *post_change_javascript;

	return_list = list_new_list();

	if ( attribute->post_change_javascript
	&&   *attribute->post_change_javascript )
	{
		post_change_javascript = attribute->post_change_javascript;
	}
	else
	{
		post_change_javascript = folder_post_change_javascript;
	}

	if ( prompt_data_element_only || attribute->omit_update )
	{
		update_yn = 'n';
	}

	element_list =
		appaserver_library_update_lookup_attribute_element_list(
				update_yn,
				primary_key_list,
				attribute->exclude_permission_list,
				attribute->attribute_name,
				attribute->datatype,
				attribute->width,
				post_change_javascript,
				attribute->on_focus_javascript_function,
				is_primary_attribute );

	if ( element_list && list_length( element_list ) )
	{
		list_append_list( return_list, element_list );
		(*objects_outputted)++;
	}

	return return_list;
}
#endif

#ifdef NOT_DEFINED
LIST *appaserver_library_insert_attribute_element_list(
			int *objects_outputted,
			LIST *attribute_list,
			char *attribute_name,
			LIST *posted_attribute_name_list,
			boolean is_primary_attribute,
			char *folder_post_change_javascript,
			char *application_name )
{
	ATTRIBUTE *attribute;
	LIST *element_list = {0};
	LIST *return_list;
	char *post_change_javascript;

	attribute =
		attribute_seek_attribute( 
			attribute_name,
			attribute_list );
	
	if ( !attribute )
	{
		char msg[ 1024 ];

		sprintf(msg,
		"%s.%s() cannot find attribute = (%s)\n",
			__FILE__,
			__FUNCTION__,
			attribute_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	if ( appaserver_exclude_permission(
		attribute->exclude_permission_list,
		"insert" ) )
	{
		return (LIST *)0;
	}

	if ( strcmp( attribute->datatype, "timestamp" ) == 0 )
	{
		return (LIST *)0;
	}

	return_list = list_new_list();

	if ( attribute->post_change_javascript
	&&   *attribute->post_change_javascript )
	{
		post_change_javascript = attribute->post_change_javascript;
	}
	else
	{
		post_change_javascript = folder_post_change_javascript;
	}

	element_list =
	appaserver_library_with_attribute_insert_attribute_element_list(
			attribute->attribute_name,
			attribute->datatype,
			attribute->width,
			post_change_javascript,
			attribute->on_focus_javascript_function,
			posted_attribute_name_list,
			is_primary_attribute,
			attribute->omit_update );

	if ( element_list && list_length( element_list ) )
	{
		list_append_list( return_list, element_list );
		(*objects_outputted)++;
	}

	return return_list;
}
#endif

#ifdef NOT_DEFINED
LIST *appaserver_library_with_attribute_insert_attribute_element_list(
			char *attribute_name,
			char *datatype,
			int width,
			char *post_change_javascript,
			char *on_focus_javascript_function,
			LIST *posted_attribute_name_list,
			boolean is_primary_attribute,
			boolean omit_update )
{
	LIST *return_list;
	APPASERVER_ELEMENT *element;

	if ( !datatype ) return (LIST *)0;

	return_list = list_new();

	if ( posted_attribute_name_list
	&&   list_exists_string(
		attribute_name,
		posted_attribute_name_list ) )
	{
		element = element_appaserver_new(
				hidden, 
				attribute_name );
	}
	else
	if ( strcmp( datatype, "notepad" ) == 0 )
	{
		element = element_appaserver_new(
				notepad,
				attribute_name );

		element_notepad_set_attribute_width(
				element->notepad,
				width );

		element->notepad->heading = element->name;
	}
	else
	if ( strcmp( datatype, "password" ) == 0 )
	{
		element = element_appaserver_new(
				password,
				attribute_name );

		element_password_set_attribute_width(
				element->password,
				width );

		element_password_set_heading(
				element->password,
				element->name );
	}
	else
	if ( strcmp( datatype, "hidden_text" ) == 0 )
	{
		element = element_appaserver_new(
				hidden,
				attribute_name );
	}
	else
	if ( process_parameter_list_element_name_boolean( attribute_name ) )
	{
		element =
			element_yes_no_element(
				attribute_name,
				(char *)0 /* prepend_folder_name */,
				post_change_javascript,
				0 /* not with_is_null */,
				0 /* not with_not_null */ );
	}
	else
	if ( strcmp( datatype, "reference_number" ) == 0 )
	{
		element = element_appaserver_new(
				reference_number,
				attribute_name );

		element_reference_number_set_attribute_width(
				element->reference_number,
				width );

		if ( is_primary_attribute )
		{
			char heading[ 128 ];
			sprintf( heading, "*%s", element->name );
			element_reference_number_set_heading(
					element->reference_number,
					strdup( heading ) );
		}
		else
		{
			element_reference_number_set_heading(
					element->reference_number,
					element->name );
		}
		element->reference_number->omit_update = omit_update;
	}
	else
	{
		if ( strcmp( datatype, "date" ) == 0
		||   strcmp( datatype, "current_date" ) == 0 )
		{
			element = element_appaserver_new(
					element_date,
					attribute_name );
		}
		else
		if ( strcmp( datatype, "current_date_time" ) == 0 )
		{
			element = element_appaserver_new(
					element_current_date_time,
					attribute_name );
		}
		else
		{
			element = element_appaserver_new(
					text_item,
					attribute_name );
		}

		element_text_item_set_attribute_width(
				element->text_item, 
				width );

		element->text_item->state = "insert";

		if ( is_primary_attribute )
		{
			char heading[ 128 ];
			sprintf( heading, "*%s", element->name );
			element_text_item_set_heading(
					element->text_item,
					strdup( heading ) );
		}
		else
		{
			element_text_item_set_heading(
					element->text_item,
					element->name );
		}

		/* Note: must include validate_date.js */
		/* ----------------------------------- */
		if ( strcmp( datatype, "time" ) == 0
		||   strcmp( datatype, "current_time" ) == 0 )
		{
			post_change_javascript =
				attribute_append_post_change_javascript(
					post_change_javascript,
					"validate_time(this)",
					1 /* place_first */ );
		}
		else
		if ( strcmp( datatype, "date" ) == 0
		||   strcmp( datatype, "current_date" ) == 0 )
		{
			post_change_javascript =
				attribute_append_post_change_javascript(
					post_change_javascript,
					"validate_date(this)",
					1 /* place_first */ );
		}

		if ( post_change_javascript
		&&   *post_change_javascript )
		{
			element->text_item->post_change_javascript =
				post_change_javascript;
		}
		if ( on_focus_javascript_function
		&&   *on_focus_javascript_function )
		{
			element->text_item->on_focus_javascript_function =
				on_focus_javascript_function;
		}
	}

	list_append_pointer( return_list, element );
	return return_list;

}
#endif

boolean appaserver_library_validate_begin_end_date(
			char **begin_date,
			char **end_date,
			DICTIONARY *post_dictionary )
{
	char buffer[ 128 ];
	int i;
	static char new_begin_date[ 16 ];
	static char new_end_date[ 16 ];

	if ( !begin_date && !end_date ) return 0;

	if ( character_exists( *begin_date, '/' ) ) return 1;
	if ( character_exists( *end_date, '/' ) ) return 1;

	if ( !*begin_date && dictionary_length( post_dictionary ) )
	{
		dictionary_index_data(
			begin_date,
			post_dictionary,
			"begin_date",
			0 );
	}

	if ( !*end_date && dictionary_length( post_dictionary ) )
	{
		dictionary_index_data(
			end_date,
			post_dictionary,
			"end_date",
			0 );
	}

	if ( !*begin_date || strcmp( *begin_date, "begin_date" ) == 0 )
	{
		*begin_date = "1901-01-01";
	}

	if ( !*end_date || strcmp( *end_date, "end_date" ) == 0 )
	{
		if ( strcmp( *begin_date, "1901-01-01" ) == 0 )
		{
			char sys_string[ 128 ];

			strcpy( sys_string, "date.e 0 | piece.e ':' 0" );
			*end_date = pipe2string( sys_string );
		}
		else
		{
			*end_date = *begin_date;
		}
	}

	strcpy( new_begin_date, *begin_date );
	strcpy( new_end_date, *end_date );

	piece( buffer, '-', new_begin_date, 0 );
	if ( strlen( buffer ) != 4 ) return 0;

	/* Don't allow for 0000-00-00 */
	/* -------------------------- */
	if ( !atoi( buffer ) ) return 0;

	if ( !piece( buffer, '-', new_begin_date, 1 ) )
	{
		strcat( new_begin_date, "-01-01" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 12 ) return 0;
	}

	if ( !piece( buffer, '-', new_begin_date, 2 ) )
	{
		strcat( new_begin_date, "-01" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 31 ) return 0;
	}

	if ( !*new_end_date || strcmp( new_end_date, "end_date" ) == 0 )
	{
		strcpy( new_end_date, new_begin_date );
	}

	piece( buffer, '-', new_end_date, 0 );
	if ( strlen( buffer ) != 4 ) return 0;
	if ( !atoi( buffer ) ) return 0;

	if ( !piece( buffer, '-', new_end_date, 1 ) )
	{
		strcat( new_end_date, "-12-31" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 12 ) return 0;
	}

	if ( !piece( buffer, '-', new_end_date, 2 ) )
	{
		strcat( new_end_date, "-31" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 31 ) return 0;
	}

	*begin_date = new_begin_date;
	*end_date = new_end_date;

	if ( strcmp( new_begin_date, new_end_date ) > 0 )
		return 0;
	else
		return 1;

}

#ifdef NOT_DEFINED
LIST *appaserver_library_primary_data_list(
			DICTIONARY *post_dictionary,
			LIST *attribute_list )
{
	LIST *primary_key_list;
	LIST *primary_data_list;
	char *data;

	primary_data_list = list_new_list();

	if ( !attribute_list
	||   !list_length( attribute_list ) )
	{
		return primary_data_list;
	}

	primary_key_list =
		folder_get_primary_key_list(
			attribute_list );

	if ( !list_rewind( primary_key_list ) )
		return primary_data_list;

	do {
		if ( !( data =
			dictionary_index_zero(
				post_dictionary,
				list_get_pointer(
					primary_key_list ) ) ) )
		{
			return list_new_list();
		}

		list_append_pointer( primary_data_list, data );
	} while( list_next( primary_key_list ) );
	return primary_data_list;
}
#endif

char *appaserver_library_server_address( void )
{
	char server_address[ 128 ] = {0};

	if ( !environ_name_to_value( server_address, "HTTP_HOST" ) )
	{
		if ( !environ_name_to_value(
					server_address,
					"SERVER_NAME" ) )
		{
			if ( !environ_name_to_value(
					server_address,
					"SERVER_ADDR" ) )
			{
				environ_name_to_value(
						server_address,
						"HOSTNAME" );
			}
		}
	}

	return strdup( server_address );
}

void appaserver_library_output_ftp_prompt(
				char *output_filename,
				char *prompt,
				char *target,
				char *mime_type )
{
	char mime_type_prompt[ 128 ];

	if ( mime_type && *mime_type )
	{
		sprintf( mime_type_prompt,
			 "type=\"%s\"",
			 mime_type );
	}
	else
	{
		*mime_type_prompt = '\0';
	}

	if ( target && *target )
	{
		printf(
		"<br><a %s href=\"%s\" target=\"%s\">%s</a>\n",
			mime_type_prompt,
			output_filename,
			target,
			prompt );
	}
	else
	{
		printf(
		"<br><a %s href=\"%s\">%s</a>\n",
			mime_type_prompt,
			output_filename,
			prompt );
	}
	
}

char *appaserver_library_http_prompt(
			char *cgi_directory,
		 	char *server_address,
			char ssl_support_yn,
			char prepend_http_protocol_yn )
{
	static char http_prompt[ 512 ];

	if ( prepend_http_protocol_yn == 'y' )
	{
		if ( ssl_support_yn == 'y' )
		{
			sprintf( http_prompt,
			 	"https://%s%s",
			 	server_address,
			 	cgi_directory );
		}
		else
		{
			sprintf( http_prompt,
			 	"http://%s%s",
			 	server_address,
			 	cgi_directory );
		}
	}
	else
	{
		sprintf( http_prompt, "%s", cgi_directory );
	}

	return http_prompt;
}

char *appaserver_library_whoami( void )
{
	char sys_string[ 64 ];

	strcpy( sys_string, "whoami" );
	return pipe2string( sys_string );
}

boolean appaserver_library_from_preprompt( DICTIONARY *dictionary )
{
	char key[ 128 ];
	char *data;

	if ( !dictionary ) return 0;

	sprintf( key, "%s", FROM_PREPROMPT );

	if ( ! ( data =
			dictionary_get(
				key,
				dictionary ) ) )
	{
		return 0;
	}

	return ( strcmp( data, "yes" ) == 0 );
}

void appaserver_library_output_style_sheet(
			FILE *output_file, 
			char *application_name )
{
	char buffer[ 1024 ];
	FILE *input_file;

	fprintf( output_file, "<style>\n" );
	fflush( output_file );

	sprintf( buffer,
		 "%s/appaserver/%s/style.css",
		 appaserver_parameter_file_document_root(),
		 application_name );

	input_file = fopen( buffer, "r" );
	if ( !input_file )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 buffer );
		exit( 1 );
	}

	while( get_line( buffer, input_file ) )
		fprintf( output_file, "%s\n", buffer );

	fclose( input_file );
	fflush( stdout );
	fprintf( output_file,
		 "</style>\n" );
}

LIST *appaserver_library_trim_carrot_number(
			LIST *data_list )
{
	char *data;
	char *ptr;

	if ( !list_rewind( data_list ) )
		return data_list;

	do {
		data = list_get_pointer( data_list );
		ptr = data + strlen( data );
		while( ptr > data )
		{
			if ( *ptr == '^' )
			{
				*ptr = '\0';
				break;
			}
			ptr--;
		}
				
	} while( list_next( data_list ) );

	return data_list;
}

void appaserver_library_output_calendar_javascript( void )
{
		fflush( stdout );
		printf(
"<link rel=stylesheet type=text/css href=/%s/src/css/jscal2.css />\n"
"<link rel=stylesheet type=text/css href=/%s/src/css/border-radius.css />\n"
"<link rel=stylesheet type=text/css href=/%s/src/css/gold/gold.css />\n"
"<script type=text/javascript src=/%s/src/js/jscal2.js></script>\n"
"<script type=text/javascript src=/%s/src/js/lang/en.js></script>\n",
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY );
		fflush( stdout );
}

int appaserver_library_reference_number(
			char *application_name,
			int insert_rows_number )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "reference_number.sh \"%s\" %d 2>>%s",
		 application_name,
		 insert_rows_number,
		 appaserver_library_error_filename( application_name ) );

	return atoi( pipe2string( sys_string ) );
}

char *appaserver_library_prelookup_button_control_string(
			char *application_name,
			char *cgi_directory,
			char *server_address,
			char *login_name,
			char *session,
			char *folder_name,
			char *lookup_before_drop_down_base_folder_name,
			char *role_name,
			char *state )
{
	static char control_string[ 1024 ];
	char *use_folder_name;

	if ( lookup_before_drop_down_base_folder_name )
		use_folder_name = lookup_before_drop_down_base_folder_name;
	else
		use_folder_name = folder_name;

	sprintf(control_string,
		"%s/post_choose_folder?%s+%s+%s+%s+%s+%s",
		appaserver_library_http_prompt(
			cgi_directory,
		 	server_address,
			application_ssl_support_yn(
				application_name ),
		       application_prepend_http_protocol_yn(
				application_name ) ),
		 login_name,
		 application_name,
		 session,
		 use_folder_name,
		 role_name,
		 state );

	return control_string;
}

boolean appaserver_library_from_php( DICTIONARY *post_dictionary )
{
	char *filename;
	char *extension;

	if ( !( filename =
			dictionary_fetch(
				"filename",
				post_dictionary ) ) )
	{
		return 0;
	}

	extension = basename_get_extension( filename );
	return ( strcmp( extension, "php" ) == 0 );
}

boolean appaserver_library_application_exists(
				char *application,
				char *appaserver_error_directory )
{
	char sys_string[ 1024 ];
	char *results;

	if ( timlib_strcmp( application, "mysql" ) == 0
	||   timlib_strcmp( application, "appaserver" ) == 0
	||   timlib_strcmp( application, "root" ) == 0
	||   timlib_strcmp( application, "test" ) == 0
	||   timlib_strcmp( application, "system" ) == 0 )
	{
		return 1;
	}

	sprintf( sys_string,
		 "stat.e %s/appaserver_%s.err 2>&1 | grep '^\\.filename'",
		 appaserver_error_directory,
		 application );

	if ( ( results = pipe2string( sys_string ) ) )
		return 1;
	else
		return 0;
}

char *appaserver_library_default_role_name(
				char *application_name,
				char *login_name )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=role			"
		 "			folder=login_default_role	"
		 "			where=\"login_name = '%s'\"	",
		 application_name,
		 login_name );

	results = pipe2string( sys_string );

	if ( !results )
		return "";
	else
		return results;
}

#ifdef NOT_DEFINED
int appaserver_library_add_operations(
			LIST *element_list,
			int objects_outputted,
			LIST *operation_list,
			char *delete_isa_only_folder_name )
{
	APPASERVER_ELEMENT *element;
	OPERATION *operation;
	char heading[ 128 ];
	char buffer[ 128 ];

	if ( !operation_list || !list_reset( operation_list ) )
		return objects_outputted;

	do {
		objects_outputted++;

		operation = list_get_pointer( operation_list );

		if ( operation->delete_placeholder )
		{
			element =
				element_appaserver_new(
					empty_column,
					operation->label );

			list_set( element_list, element );

			continue;
		}

		element =
			element_appaserver_new(
				toggle_button,
				operation->
					process->
					process_name);
	
		if ( timlib_begins_string(
			operation->process->process_name,
			"delete" ) )
		{
			if ( delete_isa_only_folder_name
			&&   *delete_isa_only_folder_name
			&&   timlib_strcmp(
				operation->process->process_name,
				"delete_isa_only" ) == 0 )
				
			{
				sprintf( heading,
					 "delete %s only",
					 delete_isa_only_folder_name );

				format_initial_capital(
					heading,
					heading );

				element->toggle_button->heading =
					strdup( heading );
			}

			sprintf( buffer,
				 "/%s/trashcan.gif",
				 IMAGE_RELATIVE_DIRECTORY );

			element->toggle_button->image_source =
				strdup( buffer );

			element->toggle_button->onclick_function =
				"timlib_delete_button_warning()";
		}
		else
		if ( strcmp( operation->process->process_name,
			     "detail" ) == 0 )
		{
			sprintf( buffer,
				 "/%s/magnify_glass.gif",
				 IMAGE_RELATIVE_DIRECTORY );

			element->toggle_button->image_source =
				strdup( buffer );
		}

		list_set( element_list, element );

	} while( list_next( operation_list ) );

	return objects_outputted;
}
#endif

#ifdef NOT_DEFINED
boolean appaserver_library_exists_javascript_folder(
				char *application_name,
				char *folder_name )
{
	char where[ 128 ];
	char sys_string[ 256 ];

	sprintf( where, "folder = '%s'", folder_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=javascript_folders	"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return atoi( pipe2string( sys_string ) );
}
#endif

void appaserver_library_purge_temporary_files( char *application_name )
{
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "appaserver_purge_temporary_files.sh %s &",
		 application_name );

	if ( system( sys_string ) ){};
}

char *appaserver_library_change_state_display(
			enum preupdate_change_state preupdate_change_state )
{
	return appaserver_library_preupdate_change_state_display(
			preupdate_change_state );
}

char *appaserver_library_preupdate_change_state_display(
			enum preupdate_change_state preupdate_change_state )
{
	if ( preupdate_change_state == from_null_to_something )
		return "from_null_to_something";
	else
	if ( preupdate_change_state == from_something_to_null )
		return "from_something_to_null";
	else
	if ( preupdate_change_state == from_something_to_something_else )
		return "from_something_to_something_else";
	else
		return "no_change";

}

enum preupdate_change_state appaserver_library_preupdate_change_state(
				char *preupdate_data,
				char *postupdate_data,
				char *preupdate_placeholder_name )
{
	if ( !preupdate_data && !postupdate_data )
		return no_change;

	/* ------------------------------------------ */
	/* If a numeric attribute is being checked.   */
	/* Note: can't return from_something_to_null. */
	/* ------------------------------------------ */
	if ( !postupdate_data )
	{
		if ( !preupdate_data ||	!preupdate_placeholder_name )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: null parameters.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
		else
		if ( !*preupdate_data
		||   strcmp( preupdate_data, "select" ) == 0 )
		{
			return from_null_to_something;
		}
		else
		if ( strcmp( preupdate_data, preupdate_placeholder_name ) != 0 )
		{
			return from_something_to_something_else;
		}
		else
		{
			return no_change;
		}
	}

	if (	!preupdate_data
	||	!postupdate_data
	||	!preupdate_placeholder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: null parameters.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( preupdate_data, preupdate_placeholder_name ) == 0 )
	{
		return no_change;
	}
	else
	if ( !*preupdate_data && !*postupdate_data )
	{
		return no_change;
	}
	else
	if ( !*preupdate_data
	||   strcmp( preupdate_data, "select" ) == 0 )
	{
		return from_null_to_something;
	}
	else
	if ( !*postupdate_data
	||   strcmp( postupdate_data, "null" ) == 0
	||   strcmp( postupdate_data, "/" ) == 0 )
	{
		return from_something_to_null;
	}
	else
	{
		return from_something_to_something_else;
	}
}

LIST *appaserver_library_application_name_list(
				char *appaserver_error_directory )
{
	char sys_string[ 1024 ];

	if ( chdir( appaserver_error_directory ) == -1 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot chdir(%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 appaserver_error_directory );

		exit( 1 );
	}

	sprintf( sys_string,
		 "ls -1 appaserver_*.err 2>/dev/null	|"
		 "sed 's/^appaserver_//'		|"
		 "sed 's/\\.err$//'			|"
		 "cat					 " );

	return pipe2list( sys_string );;
}

