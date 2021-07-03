/* $APPASERVER_HOME/src_hydrology/hydrology_quick_measurement_form.c	*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "form.h"
#include "folder.h"
#include "application.h"
#include "operation.h"
#include "dictionary.h"
#include "attribute.h"
#include "query.h"
#include "decode_html_post.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "dictionary2file.h"
#include "appaserver.h"
#include "element.h"
#include "hydrology_library.h"
#include "station_datatype.h"

/* Constants */
/* --------- */
#define PROCESS_NAME			"quick_measurement_form"
#define FOLDER_NAME			"measurement"
#define INSERT_UPDATE_KEY		EDIT_FRAME
#define DROP_DOWN_NUMBER_COLUMNS	1
#define FORM_NAME			"quick_measurement_form"
#define FORM_TITLE			"DataForEver Quick Measurement Form"

/* Prototypes */
/* ---------- */
boolean valid_manipulate_agency( 
				char *application_name,
				char *login_name,
				DICTIONARY *query_dictionary );

LIST *get_filter_manipulate_agency_list(
				char *application_name,
				char *login_name );

LIST *get_element_list(
			LIST *attribute_list,
			LIST *attribute_name_list );

char *get_where_clause(	DICTIONARY *dictionary,
			char *application_name,
			char *appaserver_mount_point,
			char *station,
			char *datatype );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *session;
	char *folder_name;
	char *role_name;
	char *target_frame;
	char *state;
	char *insert_update_key;
	char *station;
	char *datatype;
	FORM *form;
	FOLDER *folder;
	QUERY *query;
	LIST *row_dictionary_list = {0};
	int number_rows_outputted = 0;
	LIST *attribute_name_list;
	DICTIONARY *post_dictionary = (DICTIONARY *)0;
	DICTIONARY *query_dictionary = (DICTIONARY *)0;
	char post_dictionary_string[ 65536 ];
	char decoded_dictionary_string[ 65536 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int output_submit_reset_buttons_in_heading = 1;
	int output_submit_reset_buttons_in_trailer = 0;

	if ( argc != 5 )
	{
		fprintf( stderr, 
		"Usage: %s login_name application session dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	strcpy( post_dictionary_string, argv[ 4 ] );

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		application_name );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	decode_html_post(
		decoded_dictionary_string, 
		post_dictionary_string );

	post_dictionary = 
		dictionary_index_string2dictionary( 
			decoded_dictionary_string );

	dictionary_appaserver_parse_multi_attribute_keys(
		post_dictionary,
		(char *)0 /* prefix */ );

	if ( ! ( station =
			dictionary_get_string(
				post_dictionary, "station" ) ) )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<p>ERROR: Please select a station/datatype.\n" );
		document_close();
		exit( 0 );
	}

	if ( ! ( datatype =
			dictionary_get_string(
				post_dictionary, "datatype" ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: dictionary_get_string(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			datatype );
		exit( 1 );
	}

	query_dictionary =
		dictionary_remove_prefix( 	
			post_dictionary,
			QUERY_STARTING_LABEL );

	query_dictionary =
		dictionary_remove_prefix(
			query_dictionary,
			QUERY_FROM_STARTING_LABEL );

	query_dictionary = dictionary_remove_index( query_dictionary );

	if ( !valid_manipulate_agency( 
			application_name,
			login_name,
			query_dictionary ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<p>Sorry: This device is assigned to another agency.\n" );

		document_close();
		exit( 0 );
	}

	folder_name = FOLDER_NAME;
	role_name = "";
	state = "";
	target_frame = EDIT_FRAME;
	insert_update_key = EDIT_FRAME;

	/* ---------------------------------------------- */
	/* Populate the list of attributes to display. 	  */
	/* Note: only measurement value will be editable. */
	/* Note: station and datatype will be hidden.     */
	/* ---------------------------------------------- */
	attribute_name_list = list_new();
	list_append_string( attribute_name_list, "measurement_date" );
	list_append_string( attribute_name_list, "measurement_time" );
	list_append_string( attribute_name_list, "measurement_value" );
	list_append_string( attribute_name_list, "station" );
	list_append_string( attribute_name_list, "datatype" );

	folder =
		folder_new_folder( 	
			application_name,
			session,
			folder_name );

	folder->attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* related_folder_list */,
			(char *)0 /* role_name */ );

	query =
		query_simple_new(
			query_dictionary,
			application_name,
			login_name,
			folder_name );

	query->query_output->where_clause =
		get_where_clause(
			query->dictionary,
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point,
			station,
			datatype );

	row_dictionary_list =
		query_row_dictionary_list(
				query->folder->application_name,
				query->query_output->select_clause,
				query->query_output->from_clause,
				query->query_output->where_clause,
				query->query_output->order_clause,
				10000 /* query->max_rows */,
				query->folder->append_isa_attribute_list,
				query->login_name );

	form = form_new( FORM_NAME, FORM_TITLE );
	form->omit_folder_name_in_title = 1;

	form_set_folder_parameters(
		form,
		state,
		login_name,
		application_name,
		session,
		folder->folder_name,
		role_name );

	/* form_set_title( form, FORM_TITLE ); */

	form->regular_element_list = 
		get_element_list( 	
			folder->attribute_list,
			attribute_name_list );

	form->post_process = "post_hydrology_quick_measurement_form";
	form_set_current_row( form, 1 );
	form_set_output_table_heading( form );
	form_set_row_dictionary_list( form, row_dictionary_list );
	form->insert_update_key = insert_update_key;
	form->target_frame = target_frame;
	form->process_id = getpid();

	document =
		document_new(
			FORM_TITLE,
			application_name );

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "null2slash" );

	document_set_output_content_type( document );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_parameter_file->appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		0 /* not with_dynarch_menu */ );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	form->table_border = 1;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->session,
		form->form_name,
		form->post_process,
		form->action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		output_submit_reset_buttons_in_heading,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* post_change_javascript */ );

	form_output_table_heading(
		form->regular_element_list,
		0 /* form_number */ );

	number_rows_outputted +=
		form_output_body(
			&form->current_reference_number,
			form->hidden_name_dictionary,
			form->output_row_zero_only,
			form->row_dictionary_list,
			form->regular_element_list,
			form->viewonly_element_list,
			dictionary2file_get_filename(
				form->process_id,
				appaserver_parameter_file->
					appaserver_data_directory,
				INSERT_UPDATE_KEY,
				FOLDER_NAME,
				(char *)0 /* optional_related_attribute... */ )
					/* spool_filename */,
			0 /* row_level_non_owner_view_only */,
			application_name,
			login_name,
			(char *)0 /* attribute_not_null_string */,
			(char *)0 /* appaserver_user_foreign_login_name */,
			(LIST *)0 /* non_edit_folder_name_list */ );

	if ( number_rows_outputted > ROWS_FOR_SUBMIT_AT_BOTTOM )
		output_submit_reset_buttons_in_trailer = 1;

	output_dictionary_as_hidden( post_dictionary );

	printf( "</table>\n" );
	printf( "<table border=0>\n" );

	form_output_trailer(
		output_submit_reset_buttons_in_trailer,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		0 /* not with_back_to_top_button */,
		0 /* form_number */,
		(LIST *)0 /* form_button_list */,
		(char *)0 /* post_change_javascript */ );

	printf( "</table>\n" );
	printf( "</form>\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
}

char *get_where_clause(	DICTIONARY *dictionary,
			char *application_name,
			char *appaserver_mount_point,
			char *station,
			char *datatype )
{
	char where_clause[ 4096 ];
	char *begin_date;
	char *end_date;

	begin_date = dictionary_get_string( dictionary, "begin_date" );
	end_date = dictionary_get_string( dictionary, "end_date" );

	hydrology_library_get_clean_begin_end_date(
		&begin_date,
		&end_date,
		application_name,
		station,
		datatype );

	if ( !appaserver_library_validate_begin_end_date(
			&begin_date,
			&end_date,
			(DICTIONARY *)0 /* post_dictionary */ ) )
	{
		document_quick_output_body(	application_name,
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	if ( !end_date )
	{
		end_date = begin_date;
	}
	else
	if ( !begin_date )
	{
		begin_date = end_date;
	}

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf( where_clause,
		 "station = '%s' and				"
		 "datatype = '%s' and				"
		 "measurement_date between '%s' and '%s'	",
		 station,
		 datatype,
		 begin_date,
		 end_date );
	}
	else
	{
		sprintf( where_clause,
		 "station = '%s' and				"
		 "datatype = '%s' and				"
		 "measurement_date = '%s'			",
		 station,
		 datatype,
		 begin_date );
	}

	return strdup( where_clause );
}

LIST *get_element_list(
			LIST *attribute_list,
			LIST *attribute_name_list )
{
	LIST *return_list;
	ATTRIBUTE *attribute;
	char *attribute_name;
	ELEMENT_APPASERVER *element = {0};

	if ( !list_rewind( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty attribute_name_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return_list = list_new();

	/* For each attribute */
	/* ------------------ */
	do {
		attribute_name = list_get( attribute_name_list );

		attribute =
			attribute_seek_attribute( 
				attribute_name,
				attribute_list );

		if ( !attribute )
		{
			fprintf(stderr,
				"%s/%s() cannot find attribute = (%s)\n",
				__FILE__,
				__FUNCTION__,
				attribute_name );
			exit( 1 );
		}

		/* ----------------------------------------- */
		/* Measurement value get a text item element */
		/* ----------------------------------------- */
		if ( strcmp( 	attribute->attribute_name, 
				"measurement_value" ) == 0 )
		{
			element = element_appaserver_new(
					text_item,
					attribute->attribute_name);

			element_text_item_set_attribute_width(
					element->text_item, 
					attribute->width );

			element->
				text_item->
				onchange_null2slash_yn = 'y';

			list_append( 	return_list,
					element,
					sizeof( ELEMENT_APPASERVER ) );
		}
		else
		/* ------------------------------------ */
		/* Measurement date and time get prompt */
		/* ------------------------------------ */
		if ( strcmp( 	attribute->attribute_name,
				"measurement_date" ) == 0 
		||   strcmp(	attribute->attribute_name,
				"measurement_time" ) == 0 )
		{
			element =
				element_appaserver_new(
					prompt_data,
					attribute->attribute_name);

			element->prompt_data->heading = element->name;

			list_append(
				return_list,
				element,
				sizeof( ELEMENT_APPASERVER ) );
		}
		else
		/* ------------------------------- */
		/* Station and datatype get hidden */
		/* ------------------------------- */
		if ( strcmp( 	attribute->attribute_name,
				"station" ) == 0 
		||   strcmp(	attribute->attribute_name,
				"datatype" ) == 0 )
		{
			element =
				element_appaserver_new(
					hidden,
					attribute->attribute_name);

			list_append( 	return_list,
					element,
					sizeof( ELEMENT_APPASERVER ) );
		}

	} while( list_next( attribute_name_list ) );

	return return_list;
}

LIST *get_filter_manipulate_agency_list(
			char *application_name,
			char *login_name )
{
	char *folder = "appaserver_user_agency";
	char where[ 256 ];
	char sys_string[ 1024 ];

	sprintf( where,
		 "login_name = '%s' and filter_manipulate_yn = 'y'",
		 login_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=agency	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 folder,
		 where );

	return pipe2list( sys_string );

}

boolean valid_manipulate_agency( 
			char *application_name,
			char *login_name,
			DICTIONARY *query_dictionary )
{
	char *station;
	char *datatype;
	char *manipulate_agency;
	LIST *filter_manipulate_agency_list;

	if ( ! ( station = dictionary_get_string(
				query_dictionary,
				"station" ) ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot find station in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 dictionary_display( query_dictionary ) );
		return 0;
	}

	datatype = dictionary_get_string( query_dictionary, "datatype" );

	manipulate_agency =
		station_datatype_get_manipulate_agency(
			application_name,
			station,
			datatype );

	filter_manipulate_agency_list =
		get_filter_manipulate_agency_list(
			application_name,
			login_name );

	if ( !list_length( filter_manipulate_agency_list ) )
		return 1;

	return ( list_exists_string( 
			manipulate_agency,
			filter_manipulate_agency_list ) );
}

