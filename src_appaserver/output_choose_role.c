/* $APPASERVER_HOME/src_appaserver/output_choose_role.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "appaserver_error.h"
#include "environ.h"
#include "frameset.h"
#include "choose_role.h"

/* Prototypes */
/* ---------- */
void output_choose_role_horizontal(
			char *application_name,
			char *session_key,
			char *login_name );

void output_choose_role_vertical(
			char *application_name,
			char *session_key,
			char *login_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s session login_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];

	if ( appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		output_choose_role_horizontal(
				application_name,
				session,
				login_name );
	}
	else
	{
		output_choose_role_vertical(
				application_name,
				session,
				login_name );
	}

	exit( 0 );

}

void output_choose_role_vertical(
			char *application_name,
			char *session,
			char *login_name )
{
	APPASERVER_ELEMENT *element;
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_server_address(),
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
		element->drop_down->post_change_javascript,
		element->drop_down->max_drop_down_size,
		element->drop_down->multi_select_element_name,
		element->drop_down->onblur_javascript_function,
		(char *)0 /* background_color */,
		-1 /* date_piece_offset */,
		element->drop_down->no_initial_capital,
		element->drop_down->readonly,
		0 /* tab_index */,
		element->drop_down->state );

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

void output_choose_role_horizontal(
			char *application_name,
			char *session,
			char *login_name )
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
