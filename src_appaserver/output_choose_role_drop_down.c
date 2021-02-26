/* src_appaserver/output_choose_role_drop_down.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "element.h"
#include "form.h"
#include "environ.h"
#include "appaserver_parameter_file.h"

#define FORM_NAME		"choose_role_drop_down"

/* Prototypes */
/* ---------- */
void output_horizontal_frameset_choose_role_drop_down(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *action_string,
			LIST *role_list,
			char *title );

void output_vertical_frameset_choose_role_drop_down(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *action_string,
			LIST *role_list,
			DICTIONARY *hidden_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *title;
	LIST *role_list;
	DICTIONARY *hidden_dictionary = dictionary_new();
	char action_string[ 512 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s ignored session login_name role title role_comma_list\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	title = argv[ 5 ];

	sprintf(	action_string,
			"%s/post_choose_role_drop_down",
			appaserver_library_get_http_prompt(
				appaserver_parameter_file_get_cgi_directory(),
				appaserver_library_get_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ) );

	role_list = list_string2list( argv[ 6 ], ',' );

	dictionary_set_string(	hidden_dictionary,
				"application",
				application_name );

	dictionary_set_string(	hidden_dictionary,
				"session",
				session );

	dictionary_set_string(	hidden_dictionary,
				"login_name",
				login_name );

	dictionary_set_string(	hidden_dictionary,
				"title",
				title );

	if ( appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		output_horizontal_frameset_choose_role_drop_down(
				application_name,
				session,
				login_name,
				role_name,
				action_string,
				role_list,
				title );
	}
	else
	{
		output_vertical_frameset_choose_role_drop_down(
				application_name,
				session,
				login_name,
				role_name,
				action_string,
				role_list,
				hidden_dictionary );
	}

	exit( 0 );

}

void output_vertical_frameset_choose_role_drop_down(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *action_string,
			LIST *role_list,
			DICTIONARY *hidden_dictionary )
{
	ELEMENT_APPASERVER *element;
	FORM *form;
	char post_change_javascript[ 512 ];
	LIST *option_data_list = list_new();

	form = form_new_form();
	form->form_name = FORM_NAME;

	form_output_heading(
		login_name,
		application_name,
		session,
		form->form_name,
		(char *)0 /* post_process */,
		action_string,
		(char *)0 /* folder_name */,
		(char *)0 /* role_name */,
		(char *)0 /* state */,
		(char *)0 /* insert_update_key */,
		MENU_FRAME /* target_frame is menu */,
		0 /* output_submit_reset_buttons */,
		0 /* not with_prelookup_skip_button */,
		(char *)0 /* submit_control_string */,
		0 /* table_border */,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
			form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* post_change_javascript */ );

	element =
		element_appaserver_new(
			drop_down,
			CHOOSE_ROLE_DROP_DOWN_ELEMENT_NAME );

	sprintf( post_change_javascript, "%s.submit()", FORM_NAME );
	element->drop_down->post_change_javascript =
		strdup( post_change_javascript );

	if ( *role_name && strcmp( role_name, "role" ) != 0 )
		element->drop_down->initial_data = role_name;
	else
		role_name = "";

	if ( list_reset( role_list ) )
	{
		do {
			role_name = list_get_string( role_list );

			list_append_pointer(
				option_data_list,
				(char *)role_name );

		} while( list_next( role_list ) );
	}

	element_drop_down_output(
			stdout,
			element->name,
			option_data_list,
			(LIST *)0 /* option_label_list */,
			element->drop_down->number_columns,
			element->drop_down->multi_select,
			0 /* row */,
			element->drop_down->initial_data,
			element->drop_down->output_null_option,
			element->drop_down->output_not_null_option,
			element->drop_down->output_select_option,
			element->drop_down->folder_name,
			element->drop_down->post_change_javascript,
			element->drop_down->max_drop_down_size,
			element->drop_down->multi_select_element_name,
			element->drop_down->onblur_javascript_function,
			(char *)0 /* background_color */,
			-1 /* date_piece_offset */,
			element->drop_down->no_initial_capital,
			element->drop_down->readonly,
			0 /* tab_index */,
			element->drop_down->state,
			element->drop_down->attribute_width );

	output_dictionary_as_hidden( hidden_dictionary );

	printf( "</table>\n" );
	printf( "<table border=0>\n" );

	form_output_trailer(
		0 /* output_submit_reset_buttons */,
		0 /* output_insert_flag */,
		(char *)0 /* submit_control_string */,
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

}

void output_horizontal_frameset_choose_role_drop_down(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *action_string,
			LIST *role_list,
			char *title )
{
	char *new_role_name;
	char buffer[ 128 ];

	if ( list_length( role_list ) == 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty role_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	else
	if ( list_length( role_list ) == 1 )
	{
		return;
	}

	printf(
"	<li>\n"
"        <a><span class=%s>Role</span></a>\n"
"	<ul>\n",
		HORIZONTAL_MENU_CLASS );

	list_rewind( role_list );

	do {
		new_role_name = list_get_pointer( role_list );

		if ( strcmp( role_name, new_role_name ) != 0 )
		{
			printf(
"		<li>\n"
"        	<a href=\"%s?%s+%s+%s+%s+%s+%s\" target=\"%s\">\n"
"		<span class=%s>%s</span></a>\n",
			action_string,
			application_name,
			"database",
			session,
			login_name,
			new_role_name,
			title,
			PROMPT_FRAME /* target */,
			HORIZONTAL_MENU_CLASS,
			format_initial_capital( buffer, new_role_name ) );
		}

	} while( list_next( role_list ) );

	printf(
"	</ul>\n" );

}

