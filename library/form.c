/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "operation.h"
#include "list.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "application_constant.h"
#include "dictionary_separate.h"
#include "widget.h"
#include "javascript.h"
#include "button.h"
#include "appaserver.h"
#include "form.h"

char *form_next_reference_number(
		int *form_current_reference_number )
{
	int current_reference_number;
	static char return_reference_number[ 128 ];

	current_reference_number = *form_current_reference_number;

	if ( !current_reference_number ) current_reference_number = 1;
	*form_current_reference_number = current_reference_number + 1;

	sprintf(return_reference_number, 
		"%d", 
		current_reference_number );

	return return_reference_number;
}

char *form_title_tag( char *title )
{
	char title_tag[ 256 ];

	if ( !title )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: title is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title_tag,
		"<h1>%s</h1>\n",
		title );

	return strdup( title_tag );
}

char *form_tag(	char *form_name,
		char *action_string,
		char *target_frame )
{
	char tag[ STRING_4K ];
	char *ptr = tag;

	if ( !action_string )
	{
		char message[ 128 ];

		sprintf(message, "action_string is empty" );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !target_frame )
	{
		char message[ 128 ];

		sprintf(message, "target_frame is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<form enctype=\"multipart/form-data\" method=post" );

	if ( form_name && *form_name )
	{
		ptr += sprintf(
			ptr,
			" id=\"%s\"",
			form_name );
	}

	ptr += sprintf(
		ptr,
		" target=\"%s\"",
		target_frame );

	if ( strlen( tag ) +
	     strlen( action_string ) +
	     11 >= STRING_4K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_4K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		" action=\"%s\">",
		action_string );

	return strdup( tag );
}

char *form_close_tag( void )
{
	return "</form>";
}

char *form_verify_notepad_widths_javascript(
		char *form_name,
		LIST *widget_container_list )
{
	WIDGET_CONTAINER *widget_container;
	char javascript[ 1024 ];
	char *ptr = javascript;
	boolean got_one = 0;

	if ( !list_rewind( widget_container_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"verify_attribute_widths('" );

	do {
		widget_container = list_get( widget_container_list );

		if ( widget_container->widget_type != notepad ) continue;

		if ( !widget_container->notepad->attribute_size ) continue;

		if ( !got_one )
			got_one = 1;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			widget_container->notepad->widget_name );

	} while ( list_next( widget_container_list ) );

	if ( !got_one ) return (char *)0;

	ptr += sprintf( ptr, "','" );
	got_one = 0;

	list_rewind( widget_container_list );

	do {
		widget_container = list_get( widget_container_list );

		if ( widget_container->widget_type != notepad ) continue;

		if ( !widget_container->notepad->attribute_size ) continue;

		if ( !got_one )
			got_one = 1;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%d",
			widget_container->notepad->attribute_size );

	} while( list_next( widget_container_list ) );

	ptr += sprintf(
		ptr,
		"','%s')",
		form_name );

	return strdup( javascript );
}

char *form_cookie_key(
		char *form_name,
		char *folder_name )
{
	static char key[ 128 ];

	if ( !form_name || !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"<%s_%s>",
		form_name,
		folder_name );

	return key;
}

char *form_cookie_multi_key(
		char *form_name,
		char *folder_name )
{
	static char key[ 128 ];

	if ( !form_name || !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"<multi_%s_%s>",
		form_name,
		folder_name );

	return key;
}

char *form_multi_select_all_javascript(
		LIST *widget_container_list )
{
	WIDGET_CONTAINER *widget_container;
	char javascript[ 1024 ];
	char *ptr = javascript;

	if ( !list_rewind( widget_container_list ) ) return (char *)0;

	do {
		widget_container = list_get( widget_container_list );

		if ( widget_container->widget_type != multi_drop_down )
			continue;

		if ( !widget_container->widget_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: widget_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != javascript ) ptr += sprintf( ptr, " && " );

		ptr += sprintf(
			ptr,
			"post_change_multi_select_all('%s')",
			widget_container->widget_name );

	} while ( list_next( widget_container_list ) );

	if ( ptr == javascript )
		return NULL;
	else
		return strdup( javascript );
}

char *form_ignore_name(
		char *dictionary_separate_ignore_prefix,
		char *relation_name )
{
	char name[ 128 ];

	if ( !dictionary_separate_ignore_prefix
	||   !relation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		dictionary_separate_ignore_prefix,
		relation_name );

	return strdup( name );
}

char *form_no_display_name(
		char *dictionary_separate_no_display_prefix,
		char *attribute_name )
{
	char name[ 128 ];

	if ( !dictionary_separate_no_display_prefix
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		dictionary_separate_no_display_prefix,
		attribute_name );

	return strdup( name );
}

char *form_background_color( void )
{
	static int cycle_count = 0;
	static char **background_color_array = {0};
	static int background_color_array_length = 0;
	char *background_color;

	if ( !background_color_array )
	{
		background_color_array =
			form_background_color_array(
				&background_color_array_length );
	}

	if ( !background_color_array_length )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty background_color_array.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	background_color = background_color_array[ cycle_count ];

	if ( ++cycle_count == background_color_array_length )
		cycle_count = 0;

	return background_color;
}

char **form_background_color_array(
		int *background_color_array_length )
{
	static char **background_color_array;
	char *application_constant_color;
	APPLICATION_CONSTANT *application_constant;

	background_color_array =
		(char **)calloc(FORM_MAX_BACKGROUND_COLOR_ARRAY,
				sizeof ( char * ) );

	/* Safely returns */
	/* -------------- */
	application_constant = application_constant_new();

	if ( ( application_constant_color =
		application_constant_get(
			"color1",
			application_constant->dictionary ) ) )
	{
		background_color_array[ 0 ] = application_constant_color;
		*background_color_array_length = 1;

		if ( ( application_constant_color =
			application_constant_get(
				"color2",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 1 ] =
				application_constant_color;
			*background_color_array_length = 2;
		}
		else
		{
			background_color_array[ 1 ] = "white";
		}

		if ( ( application_constant_color =
			application_constant_get(
				"color3",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 2 ] =
				application_constant_color;
			*background_color_array_length = 3;
		}
		else
		{
			background_color_array[ 2 ] = "white";
		}

		if ( ( application_constant_color =
			application_constant_get(
				"color4",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 3 ] =
				application_constant_color;
			*background_color_array_length = 4;
		}
		else
		{
			background_color_array[ 3 ] = "white";
		}

		if ( ( application_constant_color =
			application_constant_get(
				"color5",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 4 ] =
				application_constant_color;
			*background_color_array_length = 5;
		}
		else
		{
			background_color_array[ 4 ] = "white";
		}

		if ( ( application_constant_color =
			application_constant_get(
				"color6",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 5 ] =
				application_constant_color;
			*background_color_array_length = 6;
		}
		else
		{
			background_color_array[ 5 ] = "white";
		}

		if ( ( application_constant_color =
			application_constant_get(
				"color7",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 6 ] =
				application_constant_color;
			*background_color_array_length = 7;
		}
		else
		{
			background_color_array[ 6 ] = "white";
		}


		if ( ( application_constant_color =
			application_constant_get(
				"color8",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 7 ] =
				application_constant_color;
			*background_color_array_length = 8;
		}
		else
		{
			background_color_array[ 7 ] = "white";
		}

		if ( ( application_constant_color =
			application_constant_get(
				"color9",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 8 ] =
				application_constant_color;
			*background_color_array_length = 9;
		}
		else
		{
			background_color_array[ 8 ] = "white";
		}

		if ( ( application_constant_color =
			application_constant_get(
				"color10",
				application_constant->dictionary ) ) )
		{
			background_color_array[ 9 ] =
				application_constant_color;
			*background_color_array_length = 10;
		}
		else
		{
			background_color_array[ 9 ] = "white";
		}
	}
	else
	{
		background_color_array[ 0 ] = FORM_COLOR1;
		background_color_array[ 1 ] = FORM_COLOR2;
		background_color_array[ 2 ] = FORM_COLOR3;
		background_color_array[ 3 ] = FORM_COLOR4;
		background_color_array[ 4 ] = FORM_COLOR5;
		*background_color_array_length = 5;
	}
	return background_color_array;
}

char *form_prefix_widget_name(
		const char *prefix,
		char *widget_name )
{
	char name[ 256 ];

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		name,
		sizeof ( name ),
		"%s%s",
		prefix,
		widget_name );

	return strdup( name );
}

char *form_html(char *form_tag,
		char *widget_container_list_html,
		char *hidden_html,
		char *form_close_tag )
{
	char html[ STRING_704K ];

	if ( !form_tag
	||   !widget_container_list_html
	||   !form_close_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	string_strlen( form_tag ) +
		string_strlen( widget_container_list_html ) +
		string_strlen( hidden_html ) +
		string_strlen( form_close_tag ) >= STRING_704K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_704K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s",
		form_tag,
		widget_container_list_html,
		(hidden_html) ? hidden_html : "",
		form_close_tag );

	return strdup( html );
}

int form_increment_number( void )
{
	static int increment_number;

	return ++increment_number;
}

char *form_name(
		const char *label,
		int form_increment_number )
{
	char name[ 128 ];

	if ( !form_increment_number )
	{
		char message[ 128 ];

		sprintf(message, "form_increment_number is zero." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		name,
		sizeof ( name ),
		"%s_%d",
		label,
		form_increment_number );

	return strdup( name );
}

