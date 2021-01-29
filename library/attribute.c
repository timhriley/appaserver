/* $APPASERVER_HOME/library/attribute.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver attribute ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "attribute.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "query.h"
#include "environ.h"
#include "process_parameter_list.h"
#include "appaserver.h"
#include "element.h"
#include "boolean.h"
#include "date_convert.h"

ATTRIBUTE *attribute_calloc( void )
{
	ATTRIBUTE *a = (ATTRIBUTE *)calloc( 1, sizeof( ATTRIBUTE ) );
	if ( !a )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate a new attribute.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return a;
}

ATTRIBUTE *attribute_new( char *attribute_name )
{
	ATTRIBUTE *a = attribute_calloc();

	a->attribute_name = attribute_name;
	return a;
}

ATTRIBUTE *attribute_new_attribute(
				char *folder_name,
				char *attribute_name,
				char *datatype,
				int width,
				int primary_key_index,
				int display_order,
				int float_decimal_places,
				boolean omit_insert,
				boolean omit_insert_prompt,
				boolean omit_update,
				char *hint_message,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				boolean additional_unique_index,
				boolean additional_index,
				boolean lookup_histogram_output,
				boolean lookup_time_chart_output,
				boolean lookup_required,
				boolean insert_required,
				boolean appaserver )
{
	ATTRIBUTE *a = attribute_new( attribute_name );

	a->folder_name = folder_name;
	a->datatype = datatype;
	a->width = width;
	a->primary_key_index = primary_key_index; 
	a->display_order = display_order;
	a->float_decimal_places = float_decimal_places;
	a->omit_insert = omit_insert;
	a->omit_insert_prompt = omit_insert_prompt;
	a->omit_update = omit_update;
	a->hint_message = hint_message;
	a->post_change_javascript = post_change_javascript;
	a->on_focus_javascript_function = on_focus_javascript_function;
	a->additional_unique_index = additional_unique_index;
	a->additional_index = additional_index;
	a->lookup_histogram_output = lookup_histogram_output;
	a->lookup_time_chart_output = lookup_time_chart_output;
	a->lookup_required = lookup_required;
	a->insert_required = insert_required;
	a->appaserver = appaserver;
	return a;
}

ATTRIBUTE *attribute_load_folder_attribute(
					char *application_name,
					char *folder_name,
					char *attribute_name )
{
	LIST *attribute_list;

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder_name,
			attribute_name,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	return (ATTRIBUTE *)list_get_first_element( attribute_list );
}

ATTRIBUTE *attribute_load_attribute(	char *application_name,
					char *attribute_name )
{
	LIST *attribute_list;

	attribute_list =
		attribute_get_attribute_list(
			application_name,
			(char *)0 /* folder_name */,
			attribute_name,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	return (ATTRIBUTE *)list_get_first_element( attribute_list );
}

ATTRIBUTE *attribute_seek(
			char *attribute_name,
			LIST *attribute_list )
{
	return attribute_seek_attribute(
			attribute_name,
			attribute_list );
}

ATTRIBUTE *attribute_seek_attribute(
			char *attribute_name,
			LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get_pointer( attribute_list );

			if ( timlib_strcmp(
					attribute->attribute_name,
					attribute_name ) == 0 )
			{
				return attribute;
			}

		} while( list_next( attribute_list ) );
	}
	return (ATTRIBUTE *)0;
}

char *attribute_display( ATTRIBUTE *attribute )
{
	static char buffer[ 2048 ];

	sprintf(	buffer,
		"attribute = %s.%s, datatype = %s, width = %d,"
		" primary index = %d, display order = %d,"
		" omit_insert_prompt = %d, omit_insert = %d, omit_update = %d,"
		" hint = (%s)",
			(attribute->folder_name) ? attribute->folder_name
						 : "null",
			(attribute->attribute_name)
						 ? attribute->attribute_name
						 : "null",
			(attribute->datatype)	 ? attribute->datatype
						 : "null",
			attribute->width,
			attribute->primary_key_index,
			attribute->display_order,
			attribute->omit_insert_prompt,
			attribute->omit_insert,
			attribute->omit_update,
			attribute->hint_message );

	if ( list_length( attribute->exclude_permission_list ) )
	{
		sprintf( buffer + strlen( buffer ),
			 ", exclude permission: %s\n",
			 list_display( attribute->exclude_permission_list ) );
	}
	else
	{
		sprintf( buffer + strlen( buffer ), "\n" );
	}

	return buffer;

}

char *attribute_list_display( LIST *attribute_list )
{
	char buffer[ 65536 ];
	char *buffer_ptr = buffer;
	ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_list ) ) return "";

	do {
		attribute = (ATTRIBUTE *)list_get_pointer( attribute_list );

		buffer_ptr += sprintf(	buffer_ptr,
					"%s",
					attribute_display( attribute ) );

	} while( list_next( attribute_list ) );

	return strdup( buffer );

}

char *attribute_get_database_datatype(	char *datatype,
					int width,
					int float_decimal_places,
					int primary_key_index )
{
	char buffer[ 128 ];

	if ( !datatype || !*datatype )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty datatype.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( strcmp( datatype, "timestamp" ) == 0 )
	{
			strcpy( buffer, "timestamp" );
	}
	else
	if ( strcmp( datatype, "password" ) == 0 )
	{
		sprintf( buffer,
			 "char (%d)",
			 width );
	}
	else
	if ( strcmp( datatype, "text" ) == 0
	||   strcmp( datatype, "hidden_text" ) == 0
	||   strcmp( datatype, "notepad" ) == 0
	||   strcmp( datatype, "http_filename" ) == 0 )
	{
		if ( width < 256)
		{
			sprintf(buffer,
			 	"char (%d)",
			 	width );
		}
		else
		if ( width > 255 && width < 65536 )
		{
			strcpy( buffer, "text" );
		}
		else
		{
			strcpy( buffer, "longtext" );
		}
	}
	else
	if ( strcmp( datatype, "integer" ) == 0 )
	{
		strcpy( buffer, "integer" );
	}
	else
	if ( strcmp( datatype, "date" ) == 0
	||   strcmp( datatype, "current_date" ) == 0 )
	{
		strcpy( buffer, "date" );
	}
	else
	if ( strcmp( datatype, "float" ) == 0 )
	{
		sprintf( buffer,
		 	"double (%d,%d)",
		 	width,
		 	float_decimal_places );
	}
	else
	if ( strcmp( datatype, "reference_number" ) == 0 )
	{
		strcpy( buffer, "integer" );
	}
	else
	if ( strcmp( datatype, "time" ) == 0
	||   strcmp( datatype, "current_time" ) == 0 )
	{
		sprintf( buffer,
			 "char (%d)",
			 width );
	}
	else
	if ( strcmp( datatype, "date_time" ) == 0
	||   strcmp( datatype, "current_date_time" ) == 0 )
	{
		strcpy( buffer, "datetime" );
	}
	else
	{
		sprintf( buffer, "char (%d)", width );
	}

	if ( primary_key_index )
	{
		strcat( buffer, " not null" );
	}

	return strdup( buffer );

}

LIST *attribute_list_get_datatype_attribute_string_list(
					LIST *attribute_list,
					char *datatype_list_string )
{
	ATTRIBUTE *attribute;
	char *datatype_string;
	LIST *attribute_name_list = {0};
	LIST *datatype_list = string2list( datatype_list_string, ',' );

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;
	if ( !list_length( datatype_list ) ) return (LIST *)0;

	do {
		attribute = (ATTRIBUTE *)list_get_pointer( attribute_list );
		list_rewind( datatype_list );
		do {
			datatype_string = list_get_string( datatype_list );
			if ( timlib_strcmp(
				datatype_string,
				attribute->datatype ) == 0 )
			{
				if ( !attribute_name_list )
					attribute_name_list = list_new();

				list_append_pointer(
						attribute_name_list,
						attribute->attribute_name );
			}
		} while( list_next( datatype_list ) );
	} while( list_next( attribute_list ) );
	return attribute_name_list;
}

LIST *attribute_list_get_primary_datatype_attribute_string_list(
					LIST *attribute_list,
					char *datatype_list_string )
{
	ATTRIBUTE *attribute;
	char *datatype_string;
	LIST *attribute_name_list = {0};
	LIST *datatype_list;

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	datatype_list = string2list( datatype_list_string, ',' );
	if ( !list_length( datatype_list ) ) return (LIST *)0;

	do {
		attribute = (ATTRIBUTE *)list_get_pointer( attribute_list );

		if ( !attribute->primary_key_index ) continue;

		list_rewind( datatype_list );
		do {
			datatype_string = list_get_string( datatype_list );

			if ( timlib_strcmp(
				datatype_string,
				attribute->datatype ) == 0 )
			{
				if ( !attribute_name_list )
					attribute_name_list = list_new();

				list_append_pointer(
						attribute_name_list,
						attribute->attribute_name );
			}
		} while( list_next( datatype_list ) );
	} while( list_next( attribute_list ) );
	return attribute_name_list;
}

LIST *attribute_get_non_primary_attribute_list( LIST *attribute_list )
{
	LIST *non_primary_attribute_list;
	ATTRIBUTE *attribute;

	non_primary_attribute_list = list_new();

	if ( list_rewind( attribute_list ) )
		do {
			attribute = (ATTRIBUTE *)
			      list_get_pointer( attribute_list );

			if ( !attribute->primary_key_index )
			{
				list_append_pointer(
					non_primary_attribute_list,
					(char *)attribute );
			}
		} while( list_next( attribute_list ) );
	return non_primary_attribute_list;
}

LIST *attribute_using_name_list_extract_attribute_list(
					LIST *attribute_list,
					LIST *attribute_name_list )
{
	ATTRIBUTE *attribute;
	LIST *return_attribute_list = list_new();

	if ( !list_rewind( attribute_list ) )
		return return_attribute_list;

	do {
		attribute =
			(ATTRIBUTE *)
				list_get_pointer(
					attribute_list );

		if ( list_exists_string(
					attribute_name_list,
					attribute->attribute_name ) )
		{
			list_append_pointer(
					return_attribute_list,
					attribute );
		}
	} while( list_next( attribute_list ) );

	return return_attribute_list;

}

LIST *attribute_get_list(		char *application_name,
					char *folder_name,
					char *role_name )
{
	return attribute_get_attribute_list(
			application_name,
			folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );
}

LIST *attribute_append_isa_attribute_list(
					char *application_name,
					char *folder_name,
					LIST *mto1_isa_related_folder_list,
					char *role_name )
{
	LIST *attribute_list = list_new_list();
	RELATED_FOLDER *related_folder;
	LIST *isa_related_folder_attribute_list;

	attribute_append_attribute_list(
		attribute_list,
		application_name,
		folder_name,
		(char *)0 /* attribute_name */,
		role_name,
		attribute_primary_only
			/* attribute_primary_attribute_fetch */ );

	if ( list_rewind( mto1_isa_related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					mto1_isa_related_folder_list );

			isa_related_folder_attribute_list = list_new();

			attribute_append_attribute_list(
					isa_related_folder_attribute_list,
					application_name,
					related_folder->folder->folder_name,
					(char *)0 /* attribute_name */,
					role_name,
					attribute_fetch_either );

			list_append_list(
				attribute_list,
				folder_get_non_primary_attribute_list(
					isa_related_folder_attribute_list ) );

		} while( list_next( mto1_isa_related_folder_list ) );
	}

	attribute_append_attribute_list(
		attribute_list,
		application_name,
		folder_name,
		(char *)0 /* attribute_name */,
		role_name,
		attribute_non_primary
			/* attribute_primary_attribute_fetch */ );

	return attribute_list;
}

LIST *attribute_get_attribute_list(	char *application_name,
					char *folder_name,
					char *attribute_name,
					LIST *mto1_isa_related_folder_list,
					char *role_name )
{
	LIST *attribute_list = list_new_list();
	RELATED_FOLDER *related_folder;

	/* First, set the primary attributes. */
	/* ---------------------------------- */
	attribute_append_attribute_list(
		attribute_list,
		application_name,
		folder_name,
		attribute_name,
		role_name,
		attribute_primary_only
			/* attribute_primary_attribute_fetch */ );

	/* Second, set the isa attributes. */
	/* ------------------------------- */
	if ( mto1_isa_related_folder_list
	&&   list_rewind( mto1_isa_related_folder_list ) )
	{
		LIST *isa_related_folder_attribute_list;

		do {
			related_folder = (RELATED_FOLDER *)
				list_get_pointer(
					mto1_isa_related_folder_list );

			isa_related_folder_attribute_list = list_new();

			attribute_append_attribute_list(
					isa_related_folder_attribute_list,
					application_name,
					related_folder->folder->folder_name,
					(char *)0 /* attribute_name */,
					role_name,
					attribute_fetch_either );

			list_append_list(
				attribute_list,
				folder_get_non_primary_attribute_list(
					isa_related_folder_attribute_list ) );

		} while( list_next( mto1_isa_related_folder_list ) );
	}

	/* Third, set the non primary attributes */
	/* ------------------------------------- */
	attribute_append_attribute_list(
		attribute_list,
		application_name,
		folder_name,
		attribute_name,
		role_name,
		attribute_non_primary
			/* attribute_primary_attribute_fetch */ );

	return attribute_list;
}

void attribute_append_attribute_list(
			LIST *attribute_list,
			char *application_name,
			char *folder_name,
			char *attribute_name,
			char *role_name,
			enum attribute_primary_attribute_fetch
				attribute_primary_attribute_fetch )
{
	ATTRIBUTE *attribute;
	char fetched_folder_name[ 128 ];
	char fetched_attribute_name[ 128 ];
	char attribute_datatype[ 128 ];
	char width[ 128 ];
	char float_decimal_places[ 128 ];
	char display_order[ 32 ];
	char primary_key_index[ 32 ];
	char omit_insert_yn[ 8 ];
	char omit_insert_prompt_yn[ 8 ];
	char omit_update_yn[ 8 ];
	char hint_message[ 1024 ];
	char post_change_javascript[ 512 ];
	char on_focus_javascript_function[ 512 ];
	char additional_unique_index_yn[ 8 ];
	char additional_index_yn[ 8 ];
	char lookup_required_yn[ 8 ];
	char insert_required_yn[ 8 ];
	char lookup_histogram_output_yn[ 8 ];
	char lookup_time_chart_output_yn[ 8 ];
	char appaserver_yn[ 8 ];
	char *record;
	LIST *attribute_record_list;

	attribute_record_list =
		attribute_get_attribute_record_list(
				application_name,
				folder_name,
				attribute_name );

	if ( !list_reset( attribute_record_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty attribute_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	do {
		record = list_get( attribute_record_list );

		if ( !attribute_record_parse(
			fetched_folder_name,
			fetched_attribute_name,
			attribute_datatype,
			width,
			float_decimal_places,
			primary_key_index,
			display_order,
			omit_insert_yn,
			omit_insert_prompt_yn,
			omit_update_yn,
			hint_message,
			post_change_javascript,
			on_focus_javascript_function,
			additional_unique_index_yn,
			additional_index_yn,
			lookup_required_yn,
			insert_required_yn,
			lookup_histogram_output_yn,
			lookup_time_chart_output_yn,
			appaserver_yn,
			record,
			folder_name,
			attribute_name ) )
		{
			continue;
		}


		if ( attribute_primary_attribute_fetch ==
					attribute_primary_only
		&&   !atoi( primary_key_index ) )
		{
			continue;
		}

		if ( attribute_primary_attribute_fetch ==
					attribute_non_primary
		&&   atoi( primary_key_index ) )
		{
			continue;
		}

		attribute =
			attribute_new_attribute( 	
				strdup( fetched_folder_name ),
				strdup( fetched_attribute_name ),
				strdup( attribute_datatype ),
				atoi( width ),
				atoi( primary_key_index ),
				atoi( display_order ),
				atoi( float_decimal_places ),
				(*omit_insert_yn == 'y'),
				(*omit_insert_prompt_yn == 'y'),
				(*omit_update_yn == 'y'),
				strdup( hint_message ),
				strdup( post_change_javascript ),
				strdup( on_focus_javascript_function ),
				(*additional_unique_index_yn == 'y'),
				(*additional_index_yn == 'y'),
				(*lookup_histogram_output_yn == 'y'),
				(*lookup_time_chart_output_yn == 'y'),
				(*lookup_required_yn == 'y'),
				(*insert_required_yn == 'y'),
				(*appaserver_yn == 'y') );

		if ( role_name && *role_name )
		{
			attribute->exclude_permission_list =
			appaserver_get_exclude_permission_list(
					application_name,
					fetched_attribute_name,
					role_name );
		}

		list_append_pointer(	attribute_list,
					attribute );

	} while( list_next( attribute_record_list ) );
}

LIST *attribute_get_attribute_record_list(
				char *application_name,
				char *folder_name,
				char *attribute_name )
{
	char sys_string[ 1024 ];
	static LIST *folder_attribute_record_list = {0};
	static LIST *attribute_record_list = {0};

	if ( !folder_name || !*folder_name )
	{
		char *attribute_record;
		char attribute_buffer[ 128 ];

		if ( !attribute_name || !*attribute_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s(): got no folder_name nor an attribute_name\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}

		if ( !attribute_record_list )
		{
			sprintf( sys_string,
	"%s/src_appaserver/attribute_record_list.sh %s '' '%c' 2>>%s",
			appaserver_parameter_file_get_appaserver_mount_point(),
		 	application_name,
			ATTRIBUTE_DELIMITER,
			appaserver_error_get_filename(
				application_name ) );
			attribute_record_list = pipe2list( sys_string );
		}

		if ( !list_rewind( attribute_record_list ) ) return (LIST *)0;

		do {
			attribute_record =
				list_get_pointer(
					attribute_record_list );
			piece(	attribute_buffer,
				ATTRIBUTE_DELIMITER,
				attribute_record,
				1 );
			if ( strcmp( attribute_buffer, attribute_name ) == 0 )
			{
				LIST *return_list = list_new_list();
				list_append_pointer(	return_list,
							attribute_record );
				return return_list;
			}
		} while( list_next( attribute_record_list ) );
		return (LIST *)0;
	}
	else
	{
		if ( folder_attribute_record_list )
		{
			return folder_attribute_record_list;
		}

		sprintf( sys_string,
	"%s/src_appaserver/attribute_record_list.sh %s %s '%c' 2>>%s",
			appaserver_parameter_file_get_appaserver_mount_point(),
		 	application_name,
			folder_name,
			ATTRIBUTE_DELIMITER,
			appaserver_error_get_filename(
				application_name ) );
		folder_attribute_record_list = pipe2list( sys_string );
		return folder_attribute_record_list;
	}

}

boolean attribute_exists_date_attribute( LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get_pointer( attribute_list );

			if ( !attribute->datatype ) continue;

			if ( strcmp( attribute->datatype, "date" ) == 0
			||   strcmp( attribute->datatype, "current_date" ) == 0
			||   strcmp(	attribute->datatype,
					"current_date_time" ) == 0
			||   strcmp( attribute->datatype, "date_time" ) == 0 )
			{
				return 1;
			}
		} while( list_next( attribute_list ) );
	}
	return 0;
}

boolean attribute_exists_reference_number(
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get_pointer( attribute_list );

			if ( !attribute->datatype ) continue;

			if ( strcmp(	attribute->datatype,
					"reference_number" ) == 0  )
			{
				return 1;
			}
		} while( list_next( attribute_list ) );
	}
	return 0;
}

boolean attribute_list_exists_lookup_histogram_output(
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( attribute->lookup_histogram_output ) return 1;
		
		} while( list_next( attribute_list ) );
	}
	return 0;
}

boolean attribute_list_exists_lookup_time_chart_output(
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( attribute->lookup_time_chart_output ) return 1;
		
		} while( list_next( attribute_list ) );
	}
	return 0;
}

boolean attribute_exists_omit_insert_prompt( LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( attribute->omit_insert_prompt ) return 1;
		
		} while( list_next( attribute_list ) );
	}
	return 0;
}

boolean attribute_exists_omit_insert_login_name(
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( strcmp(	attribute->attribute_name,
					"login_name" ) == 0 )
			{
				return attribute->omit_insert;
			}
		
		} while( list_next( attribute_list ) );
	}
	return 0;
}

char *attribute_get_last_primary_attribute_name( LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	ATTRIBUTE *last_primary_attribute = {0};

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( attribute->primary_key_index )
			{
				if ( !last_primary_attribute )
				{
					last_primary_attribute =
						attribute;
				}
				else
				{
					if ( attribute->primary_key_index >
					     last_primary_attribute->
							primary_key_index )
					{
						last_primary_attribute =
							attribute;
					}
				}
			}
		} while( list_next( attribute_list ) );
	}
	if ( last_primary_attribute )
		return last_primary_attribute->attribute_name;
	else
		return (char *)0;
}

LIST *attribute_get_common_non_primary_attribute_name_list(
					char *application_name,
					char *folder_name,
					char *related_folder_name )
{
	LIST *return_list = list_new_list();

	related_folder_populate_common_non_primary_attribute_name_list(
				return_list,
				application_name,
				folder_name,
				related_folder_name );

	return return_list;
}

LIST *attribute_get_histogram_attribute_name_list( LIST *attribute_list )
{
	ATTRIBUTE *attribute;
    	LIST *histogram_attribute_name_list = {0};
	int first_time = 1;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( attribute->lookup_histogram_output )
			{
				if ( first_time )
				{
					histogram_attribute_name_list =
						list_new_list();
					first_time = 0;
				}

				list_append_pointer(
					histogram_attribute_name_list,
					attribute->attribute_name );
			}
		} while( list_next( attribute_list ) );
	}
	return histogram_attribute_name_list;
}

LIST *attribute_get_time_chart_attribute_name_list( LIST *attribute_list )
{
	ATTRIBUTE *attribute;
    	LIST *time_chart_attribute_name_list = {0};
	int first_time = 1;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( attribute->lookup_time_chart_output )
			{
				if ( first_time )
				{
					time_chart_attribute_name_list =
						list_new_list();
					first_time = 0;
				}

				list_append_pointer(
					time_chart_attribute_name_list,
					attribute->attribute_name );
			}
		} while( list_next( attribute_list ) );
	}
	return time_chart_attribute_name_list;
}

LIST *attribute_get_date_attribute_position_list(
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;
    	LIST *date_attribute_position_list = list_new_list();
	int position = 0;
	char position_string[ 16 ];

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( timlib_strcmp( attribute->datatype, "date" ) == 0
			||   timlib_strcmp(	attribute->datatype,
					"current_date" ) == 0 )
			{
				sprintf( position_string, "%d", position );

				list_append_pointer(
					date_attribute_position_list,
					strdup( position_string ) );
			}
			position++;
		} while( list_next( attribute_list ) );
	}
	return date_attribute_position_list;
}

LIST *attribute_get_lookup_allowed_attribute_name_list( LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *attribute_name_list;
	LIST *done_attribute_name_list;

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	attribute_name_list = list_new();
	done_attribute_name_list = list_new();

	do {
		attribute = list_get_pointer( attribute_list );

		if ( list_exists_string( attribute->exclude_permission_list,
					 "lookup" ) )
		{
			continue;
		}

		if ( list_exists_string( done_attribute_name_list,
					 attribute->attribute_name ) )
		{
			continue;
		}

		list_append_pointer(
			attribute_name_list,
			attribute->attribute_name );

		list_append_pointer(
			done_attribute_name_list,
			attribute->attribute_name );

	} while( list_next( attribute_list ) );

	return attribute_name_list;
}

LIST *attribute_folder_attribute_name_list(
			LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *attribute_name_list;
	char folder_attribute[ 256 ];

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	attribute_name_list = list_new();

	do {
		attribute = list_get( attribute_list );


		sprintf(folder_attribute,
			"%s.%s",
			attribute->folder_name,
			attribute->attribute_name );

		list_set(
			attribute_name_list,
			strdup( folder_attribute ) );

	} while( list_next( attribute_list ) );

	return attribute_name_list;
}

LIST *attribute_folder_name_list(
			LIST *attribute_list,
			char *folder_name )
{
	ATTRIBUTE *attribute;
	LIST *attribute_name_list;

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	attribute_name_list = list_new();

	do {
		attribute = list_get_pointer( attribute_list );

		if ( folder_name
		&&   strcmp( folder_name, attribute->folder_name ) != 0 )
		{
			continue;
		}

		list_set(
			attribute_name_list,
			attribute->attribute_name );

	} while( list_next( attribute_list ) );

	return attribute_name_list;
}

LIST *attribute_name_list( LIST *attribute_list )
{
	return attribute_folder_name_list( attribute_list, (char *)0 );
}

LIST *attribute_get_name_list( LIST *attribute_list )
{
	return attribute_folder_name_list( attribute_list, (char *)0 );
}

LIST *attribute_get_attribute_name_list( LIST *attribute_list )
{
	return attribute_folder_name_list( attribute_list, (char *)0 );
}

LIST *attribute_get_date_attribute_name_list( LIST *attribute_list )
{
	ATTRIBUTE *attribute;
    	LIST *attribute_name_list = list_new_list();

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( ( timlib_strcmp(
					attribute->datatype,
					"date" ) == 0 )
			||   ( timlib_strcmp(
					attribute->datatype,
					"current_date" ) == 0 )
			||   ( timlib_strcmp(
					attribute->datatype,
					"date_time" ) == 0 )
			||   ( timlib_strcmp(
					attribute->datatype,
					"current_date_time" ) == 0 ) )
			{
				list_append_pointer(
					attribute_name_list,
					attribute->attribute_name );
			}
		} while( list_next( attribute_list ) );
	}

	return attribute_name_list;
}

LIST *attribute_primary_attribute_name_list(
			LIST *attribute_list )
{
	return folder_get_primary_attribute_name_list( attribute_list );
}

LIST *attribute_get_primary_attribute_name_list(
			LIST *attribute_list )
{
	return folder_get_primary_attribute_name_list( attribute_list );
}

LIST *attribute_get_lookup_required_attribute_name_list(
				LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *lookup_required_attribute_name_list;

	lookup_required_attribute_name_list = list_new_list();

	if ( !attribute_list || !list_rewind( attribute_list ) )
		return lookup_required_attribute_name_list;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( attribute->lookup_required )
		{
			list_append_pointer(
					lookup_required_attribute_name_list,
					attribute->attribute_name );
		}
	} while( list_next( attribute_list ) );
	return lookup_required_attribute_name_list;
}

LIST *attribute_get_insert_required_attribute_name_list(
				LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *insert_required_attribute_name_list;

	insert_required_attribute_name_list = list_new_list();

	if ( !attribute_list || !list_rewind( attribute_list ) )
		return insert_required_attribute_name_list;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( attribute->insert_required )
		{
			list_append_pointer(
					insert_required_attribute_name_list,
					attribute->attribute_name );
		}
	} while( list_next( attribute_list ) );
	return insert_required_attribute_name_list;
}

LIST *attribute_get_update_lookup_exclude_attribute_name_list(
				LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *exclude_attribute_name_list;

	exclude_attribute_name_list = list_new_list();

	if ( !attribute_list || !list_rewind( attribute_list ) )
		return exclude_attribute_name_list;

	do {
		attribute = (ATTRIBUTE *)
		      list_get_pointer( attribute_list );

		if ( attribute->exclude_permission_list
		&& ( list_exists_string( attribute->exclude_permission_list,
					 "update" )
		||   list_exists_string( attribute->exclude_permission_list,
					 "lookup" ) ) )
		{
			list_append_pointer(
					exclude_attribute_name_list,
					attribute->attribute_name );
		}
	} while( list_next( attribute_list ) );
	return exclude_attribute_name_list;
}

LIST *attribute_list_subtract(		LIST *attribute_list,
					LIST *subtract_attribute_name_list )
{
	ATTRIBUTE *attribute;

	if ( !attribute_list || !list_rewind( attribute_list ) )
		return (LIST *)0;

	do {
		attribute = (ATTRIBUTE *)
		      list_get_pointer( attribute_list );

		if ( list_exists_string( subtract_attribute_name_list,
					 attribute->attribute_name ) )
		{
			list_delete_current( attribute_list );
		}
	} while( list_next( attribute_list ) );
	return attribute_list;
}

void attribute_remove_attribute_list(	LIST *attribute_list,
					char *attribute_name )
{
	ATTRIBUTE *attribute;

	if ( !attribute_list || !list_rewind( attribute_list ) )
		return;

	do {
		attribute = (ATTRIBUTE *)
		      list_get_pointer( attribute_list );

		if ( strcmp( attribute_name, attribute->attribute_name ) == 0 )
		{
			list_delete_current( attribute_list );
			return;
		}
	} while( list_next( attribute_list ) );
}

int attribute_get_primary_attribute_list_length( LIST *attribute_list )
{
	LIST *primary_attribute_name_list;
	int length;

	primary_attribute_name_list =
		attribute_get_primary_attribute_name_list(
			attribute_list );
	length = list_length( primary_attribute_name_list );
	list_free_container( primary_attribute_name_list );
	return length;
}

LIST *attribute_get_attribute_element_list(
					char *attribute_name,
					char *prepend_folder_name,
					char *datatype,
					char *post_change_javascript,
					int width,
					char *hint_message,
					boolean is_primary_attribute,
					boolean omit_push_buttons )
{
	ELEMENT_APPASERVER *element;
	char element_name[ 256 ];
	char prompt_string[ 256 ];
	char buffer[ 256 ];
	LIST *return_list;
	char ignore_element_name[ 256 ];

	return_list = list_new_list();

	/* Create a line break */
	/* ------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer(
			return_list,
			element );

	/* Create the no_display push button */
	/* --------------------------------- */
	if ( !omit_push_buttons )
	{
		sprintf( ignore_element_name, "%s%s",
			 NO_DISPLAY_PUSH_BUTTON_PREFIX,
			 attribute_name );
	
		element = element_appaserver_new(
				toggle_button,
				strdup( ignore_element_name ) );
	
		element_toggle_button_set_heading(
			element->toggle_button, "no_display" );
	
		list_append_pointer(
				return_list,
				element );
	}

	/* Create the prompt element */
	/* ------------------------- */
	if ( is_primary_attribute )
	{
		sprintf( prompt_string,
			 "*%s",
			 format_initial_capital( 
					buffer,
					attribute_name ) );
	}
	else
	{
		sprintf( prompt_string,
			 "%s",
			 format_initial_capital( 
					buffer,
					attribute_name ) );
	}

	element = element_appaserver_new(
			prompt,
			strdup( prompt_string ) );

	list_append_pointer(
			return_list,
			element );
	
	if ( process_parameter_list_element_name_boolean(
				attribute_name ) )
	{
		element =
			element_get_yes_no_element(
				attribute_name,
				prepend_folder_name,
				post_change_javascript,
				1 /* with_is_null */,
				1 /* with_not_null */ );

		list_append_pointer(
			return_list,
			element );
	}
	else
	{
		/* Create the relation operator element */
		/* ------------------------------------ */
		if ( prepend_folder_name && *prepend_folder_name )
		{
			sprintf(element_name,
			 	"%s.%s%s",
				prepend_folder_name,
			 	QUERY_RELATION_OPERATOR_STARTING_LABEL,
			 	attribute_name );
		}
		else
		{
			sprintf(element_name,
			 	"%s%s",
			 	QUERY_RELATION_OPERATOR_STARTING_LABEL,
			 	attribute_name );
		}
	
		element = element_appaserver_new(
				drop_down,
				strdup( element_name ) );
	
		element->drop_down->option_data_list =
			get_relation_operator_list( datatype );
	
		list_append_pointer(
				return_list,
				element );
	
		/* If without between "and" ... */
		/* ---------------------------- */
		if ( strcmp( datatype, "text" ) == 0
		||   strcmp( datatype, "http_filename" ) == 0 )
		{
			if ( prepend_folder_name && *prepend_folder_name )
			{
				sprintf(element_name,
				 	"%s.%s%s",
					prepend_folder_name,
				 	QUERY_FROM_STARTING_LABEL,
				 	attribute_name );
			}
			else
			{
				sprintf(element_name,
				 	"%s%s",
				 	QUERY_FROM_STARTING_LABEL,
				 	attribute_name );
			}
	
			element = element_appaserver_new(
					text_item,
					strdup( element_name ) );
		
			element->text_item->attribute_width =
					ELEMENT_TEXT_ITEM_LOOKUP_LENGTH;

			element->text_item->widget_size =
					ELEMENT_TEXT_ITEM_LOOKUP_LENGTH;

			list_append_pointer(	return_list,
						element );
		}
		else
		/* -----------*/
		/* If notepad */
		/* -----------*/
		if ( strcmp( datatype, "notepad" ) == 0 )
		{
			if ( prepend_folder_name && *prepend_folder_name )
			{
				sprintf(element_name,
					"%s.%s%s",
					prepend_folder_name,
					QUERY_FROM_STARTING_LABEL,
					attribute_name );
			}
			else
			{
				sprintf(element_name,
					"%s%s",
					QUERY_FROM_STARTING_LABEL,
					attribute_name );
			}
	
			element = element_appaserver_new(
					notepad,
					strdup( element_name ) );
		
			element->notepad->attribute_width = width;

			element->notepad->number_rows = 1;
	
			list_append_pointer(	return_list,
						element );
		}
		else
		{
			/* Create the from operand */
			/* ----------------------- */
			if ( prepend_folder_name && *prepend_folder_name )
			{
				sprintf(element_name,
					"%s.%s%s",
					prepend_folder_name,
					QUERY_FROM_STARTING_LABEL,
					attribute_name );
			}
			else
			{
				sprintf(element_name,
					"%s%s",
					QUERY_FROM_STARTING_LABEL,
					attribute_name );
			}
	
			if ( strcmp( datatype, "date" ) == 0
			||   strcmp( datatype, "current_date" ) == 0 )
			{
				element = element_appaserver_new(
						element_date,
						strdup( element_name ) );
			}
			else
			if ( strcmp( datatype, "date_time" ) == 0 )
			{
				element = element_appaserver_new(
						element_date_time,
						strdup( element_name ) );

				element->text_item->
					dont_create_current_date = 1;
			}
			else
			if ( strcmp( datatype, "current_date_time" ) == 0 )
			{
				element = element_appaserver_new(
						element_current_date_time,
						strdup( element_name ) );

				element->text_item->
					dont_create_current_date = 1;
			}
			else
			{
				element = element_appaserver_new(
						text_item,
						strdup( element_name ) );
			}

			element->text_item->attribute_width =
					ELEMENT_TEXT_ITEM_LOOKUP_LENGTH;

			element->text_item->widget_size =
					ELEMENT_TEXT_ITEM_LARGE_WIDGET_SIZE;

			list_append_pointer( 	return_list,
						element );
		
			/* Create the between "and" label */
			/* ------------------------------ */
			element = 
				element_appaserver_new( 
					prompt,
					"and" );
		
			list_append_pointer(
					return_list,
					element );
		
			/* Create the to operand */
			/* --------------------- */
			if ( prepend_folder_name && *prepend_folder_name )
			{
				sprintf(element_name,
				 	"%s.%s%s",
					prepend_folder_name,
				 	QUERY_TO_STARTING_LABEL,
				 	attribute_name );
			}
			else
			{
				sprintf(element_name,
				 	"%s%s",
				 	QUERY_TO_STARTING_LABEL,
				 	attribute_name );
			}
	
			if ( strcmp( datatype, "date" ) == 0
			||   strcmp( datatype, "current_date" ) == 0)
			{
				element = element_appaserver_new(
						element_date,
						strdup( element_name ) );
			}
			else
			if ( strcmp( datatype, "date_time" ) == 0 )
			{
				element = element_appaserver_new(
						element_date_time,
						strdup( element_name ) );

				element->text_item->
					dont_create_current_date = 1;
			}
			else
			if ( strcmp( datatype, "current_date_time" ) == 0 )
			{
				element = element_appaserver_new(
						element_current_date_time,
						strdup( element_name ) );

				element->text_item->
					dont_create_current_date = 1;
			}
			else
			{
				element = element_appaserver_new(
						text_item,
						strdup( element_name ) );
			}
		
			element->text_item->attribute_width =
					ELEMENT_TEXT_ITEM_LOOKUP_LENGTH;

			element->text_item->widget_size =
					ELEMENT_TEXT_ITEM_SMALL_WIDGET_SIZE;

			list_append_pointer( 	return_list,
						element );
		}
	}

	/* Create the hint message */
	/* ----------------------- */
	if ( hint_message && *hint_message )
	{
		element = 
			element_appaserver_new( 
				non_edit_text,
				hint_message );

		list_append_pointer(
				return_list,
				element );
	}

	return return_list;

}

int attribute_get_date_piece_offset(	LIST *attribute_list,
					LIST *exclude_attribute_name_list )
{
	ATTRIBUTE *attribute;
	int date_piece_offset = 0;

	if ( !attribute_list
	||   !list_rewind( attribute_list ) )
	{
		return -1;
	}

	do {
		attribute = list_get_pointer( attribute_list );

		if ( list_exists_string(exclude_attribute_name_list,
					attribute->attribute_name ) )
		{
			continue;
		}

		if ( timlib_strcmp( attribute->datatype, "date" ) == 0
		||   timlib_strcmp( attribute->datatype, "current_date" ) == 0 )
		{
			return date_piece_offset;
		}

		date_piece_offset++;

	} while( list_next( attribute_list ) );

	return -1;

}

char *attribute_get_reference_number_attribute_name(
				LIST *attribute_list,
				LIST *attribute_name_list )
{
	ATTRIBUTE *attribute;
	char *attribute_name;

	if ( !list_rewind( attribute_name_list ) ) return (char *)0;

	do {
		attribute_name = list_get_pointer( attribute_name_list );

		if ( !( attribute =
			attribute_seek_attribute(
				attribute_name,
				attribute_list ) ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot seek attribute = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				attribute_name );
			exit( 1 );
		}

		if ( timlib_strcmp(
			attribute->datatype,
			"reference_number" ) == 0 )
		{
			return attribute_name;
		}

	} while( list_next( attribute_name_list ) );

	return (char *)0;

}

boolean attribute_list_exists_name(
			LIST *attribute_list,
			char *attribute_name )
{
	return (boolean)
		( attribute_seek_attribute(
			attribute_name,
			attribute_list ) == 0 );

}

boolean attribute_list_exists(		LIST *attribute_list,
					char *attribute_name )
{
	ATTRIBUTE *attribute;

	attribute = attribute_seek_attribute(
			attribute_name,
			attribute_list );

	if ( attribute )
		return 1;
	else
		return 0;
}

char *attribute_append_post_change_javascript(
					char *original_post_change_javascript,
					char *post_change_javascript,
					boolean place_first )
{
	char new_post_change_javascript[ 65536 ];

	*new_post_change_javascript = '\0';

	if ( original_post_change_javascript
	&&   *original_post_change_javascript
	&&   post_change_javascript
	&&   *post_change_javascript )
	{
		if ( place_first )
		{
			sprintf(	new_post_change_javascript,
					"%s && %s",
					post_change_javascript,
					original_post_change_javascript );
		}
		else
		{
			sprintf(	new_post_change_javascript,
					"%s && %s",
					original_post_change_javascript,
					post_change_javascript );
		}
	}
	else
	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		timlib_strcpy(	new_post_change_javascript,
				post_change_javascript,
				65536 );
	}
	else
	if ( original_post_change_javascript
	&&   *original_post_change_javascript )
	{
		timlib_strcpy(	new_post_change_javascript,
				original_post_change_javascript,
				65536 );
	}

	return strdup( new_post_change_javascript );

}

LIST *attribute_get_non_primary_float_list(
			LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *attribute_float_list = {0};

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( timlib_strcmp( attribute->datatype, "float" ) == 0
		&&   attribute->display_order )
		{
			if ( !attribute_float_list )
			{
				attribute_float_list = list_new();
			}

			list_append_pointer(
				attribute_float_list,
				attribute );
		}

	} while( list_next( attribute_list ) );

	return attribute_float_list;

}

void attribute_set_dictionary_date_international(
				DICTIONARY *dictionary,
				LIST *attribute_list )
{
	LIST *key_list;
	char *key;
	char *date_string;
	char trimmed_key[ 2048 ];
	ATTRIBUTE *attribute;
	DATE_CONVERT *date;

	if ( !list_length( attribute_list ) ) return;

	key_list = dictionary_get_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return;

	do {
		key = list_get_pointer( key_list );

		trim_index( trimmed_key, key );

		if ( ! ( attribute =
				attribute_seek_attribute(
					trimmed_key,
					attribute_list ) ) )
		{
			continue;
		}

		if ( timlib_strcmp( attribute->datatype, "date" ) == 0
		||   timlib_strcmp( attribute->datatype, "current_date" ) == 0 )
		{
			date_string = dictionary_fetch( dictionary, key );

			date = date_convert_new_date_convert( 
						international,
						date_string );

			dictionary_set_pointer(
				dictionary,
				key,
				strdup( date->return_date ) );

			date_convert_free( date );
		}

	} while( list_next( key_list ) );

}

char *attribute_select(	char *application_name,
			char *folder_name,
			LIST *attribute_name_list )
{
	char *attribute_name;
	char select_clause[ 4096 ];
	char *ptr = select_clause;
	register boolean first_time = 1;

	if ( !list_rewind( attribute_name_list ) ) return (char *)0;

	do {
		attribute_name = list_get_pointer( attribute_name_list );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "," );
		}

		ptr += sprintf( ptr,
				"%s.%s",
				get_table_name(
					application_name,
					folder_name ),
				attribute_name );

	} while( list_next( attribute_name_list ) );

	return strdup( select_clause );
}

LIST *attribute_additional_unique_index_attribute_name_list(
			LIST *attribute_list )
{
	LIST *attribute_name_list = {0};
	ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	do {
		attribute = list_get( attribute_list );

		if ( attribute->additional_unique_index )
		{
			if ( !attribute_name_list )
				attribute_name_list = list_new();

			list_append_pointer(
				attribute_name_list,
				attribute->attribute_name );
		}

	} while( list_next( attribute_list ) );

	return attribute_name_list;
}

void attribute_list_remove_exclude_permission_list(
				LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_list ) ) return;

	do {
		attribute = list_get( attribute_list );
		attribute->exclude_permission_list = (LIST *)0;

	} while( list_next( attribute_list ) );
}

LIST *attribute_get_omit_insert_attribute_name_list(
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *attribute_name_list = {0};

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get_pointer( attribute_list );

			if ( attribute->omit_insert
			||   attribute->omit_insert_prompt )
			{
				if ( !attribute_name_list )
					attribute_name_list = list_new();

				list_append_pointer(
					attribute_name_list,
					attribute->attribute_name );
			}
		
		} while( list_next( attribute_list ) );
	}

	return attribute_name_list;
}

LIST *attribute_get_omit_update_attribute_name_list(
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *attribute_name_list = {0};

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get_pointer( attribute_list );

			if ( attribute->omit_update )
			{
				if ( !attribute_name_list )
					attribute_name_list = list_new();

				list_append_pointer(
					attribute_name_list,
					attribute->attribute_name );
			}
		
		} while( list_next( attribute_list ) );
	}

	return attribute_name_list;
}

int attribute_fetch_width(
			char *application_name,
			char *attribute_name )
{
	char where[ 256 ];
	char sys_string[ 1024 ];
	char *results;

	sprintf( where, "attribute = '%s'", attribute_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=width		"
		 "			folder=attribute	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
		return 0;
	else
		return atoi( results );

}

boolean attribute_exists(	char *application_name,
				char *folder_name,
				char *attribute_name )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	 	"folder_attribute_exists.sh %s %s %s",
	 	application_name,
	 	folder_name,
		attribute_name );

	return ( system( sys_string ) == 0 );

}

boolean attribute_record_parse(
				char *fetched_folder_name,
				char *fetched_attribute_name,
				char *attribute_datatype,
				char *width,
				char *float_decimal_places,
				char *primary_key_index,
				char *display_order,
				char *omit_insert_yn,
				char *omit_insert_prompt_yn,
				char *omit_update_yn,
				char *hint_message,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				char *additional_unique_index_yn,
				char *additional_index_yn,
				char *lookup_required_yn,
				char *insert_required_yn,
				char *lookup_histogram_output_yn,
				char *lookup_time_chart_output_yn,
				char *appaserver_yn,
				char *record,
				char *folder_name,
				char *attribute_name )
{
	piece(	fetched_folder_name,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_FOLDER_PIECE );

	piece(	fetched_attribute_name,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_ATTRIBUTE_PIECE );

	if ( attribute_name && *attribute_name )
	{
		if ( strcmp( attribute_name,
			     fetched_attribute_name ) != 0 )
		{
			return 0;
		}
	}

	if ( folder_name && *folder_name )
	{
		if ( strcmp( folder_name,
			     fetched_folder_name ) != 0 )
		{
			return 0;
		}
	}

	piece(	attribute_datatype,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_DATATYPE_PIECE );

	piece(	width,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_WIDTH_PIECE );

	piece(	float_decimal_places,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_FLOAT_DECIMAL_PLACES_PIECE );

	piece(	primary_key_index,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_PRIMARY_KEY_INDEX_PIECE );

	piece(	display_order,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_DISPLAY_ORDER_PIECE );

	piece(	omit_insert_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_OMIT_INSERT_YN_PIECE );

	piece(	omit_insert_prompt_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_OMIT_INSERT_PROMPT_YN_PIECE );

	piece(	omit_update_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_OMIT_UPDATE_YN_PIECE );

	piece(	hint_message,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_HINT_MESSAGE_PIECE );

	piece(	post_change_javascript,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_POST_CHANGE_JAVASCRIPT_PIECE );

	piece(	on_focus_javascript_function,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_ON_FOCUS_JAVASCRIPT_PIECE );

	piece(	additional_unique_index_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_ADDITIONAL_UNIQUE_INDEX_PIECE );

	piece(	additional_index_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_ADDITIONAL_INDEX_PIECE );

	piece(	lookup_required_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_LOOKUP_REQUIRED_PIECE );

	piece(	insert_required_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_INSERT_REQUIRED_PIECE );

	piece(	lookup_histogram_output_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_LOOKUP_HISTOGRAM_OUTPUT_YN_PIECE );

	piece(	lookup_time_chart_output_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_LOOKUP_TIME_CHART_OUTPUT_YN_PIECE );

	piece(	appaserver_yn,
		ATTRIBUTE_DELIMITER,
		record,
		ATTRIBUTE_APPASERVER_YN_PIECE );

	return 1;
}

LIST *attribute_distinct_folder_name_list( LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *distinct_folder_name_list = list_new();

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get( attribute_list );

			if ( attribute->folder_name )
			{
				list_append_unique_string(
					distinct_folder_name_list,
					attribute->folder_name );
			}
		
		} while( list_next( attribute_list ) );
	}

	return distinct_folder_name_list;
}

LIST *attribute_fetch_list(
			char *folder_name )
{
	return attribute_list( folder_name );
}

LIST *attribute_list( char *folder_name )
{
	return attribute_system_list(
			attribute_list_sys_string(
				folder_name ) );
}

char *attribute_list_sys_string( char *folder_name )
{
	char sys_string[ 1024 ];

	if ( folder_name && *folder_name )
	{
		sprintf(sys_string,
			"folder_attribute_list.sh %s",
			folder_name );
	}
	else
	{
		strcpy( sys_string, "folder_attribute_list.sh" );
	}

	return strdup( sys_string );
}

LIST *attribute_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *attribute_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			attribute_list,
			attribute_parse( input ) );
	}
	pclose( input_pipe );
	return attribute_list;
}

ATTRIBUTE *attribute_parse( char *input )
{
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char datatype[ 128 ];
	char width[ 16 ];
	char float_decimal_places[ 16 ];
	char primary_key_index[ 16 ];
	char display_order[ 16 ];
	char omit_insert_yn[ 16 ];
	char omit_insert_prompt_yn[ 16 ];
	char omit_update_yn[ 16 ];
	char hint_message[ 1024 ];
	char post_change_javascript[ 512 ];
	char on_focus_javascript_function[ 512 ];
	char additional_unique_index_yn[ 16 ];
	char additional_index_yn[ 16 ];
	char lookup_required_yn[ 16 ];
	char insert_required_yn[ 16 ];
	char lookup_histogram_output_yn[ 16 ];
	char lookup_time_chart_output_yn[ 16 ];
	char appaserver_yn[ 16 ];

	if ( !input || !*input ) return (ATTRIBUTE *)0;

	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( attribute_name, SQL_DELIMITER, input, 1 );
	piece( datatype, SQL_DELIMITER, input, 2 );
	piece( width, SQL_DELIMITER, input, 3 );
	piece( float_decimal_places, SQL_DELIMITER, input, 4 );
	piece( primary_key_index, SQL_DELIMITER, input, 5 );
	piece( display_order, SQL_DELIMITER, input, 6 );
	piece( omit_insert_yn, SQL_DELIMITER, input, 7 );
	piece( omit_insert_prompt_yn, SQL_DELIMITER, input, 8 );
	piece( omit_update_yn, SQL_DELIMITER, input, 9 );
	piece( hint_message, SQL_DELIMITER, input, 10 );
	piece( post_change_javascript, SQL_DELIMITER, input, 11 );
	piece( on_focus_javascript_function, SQL_DELIMITER, input, 12 );
	piece( additional_unique_index_yn, SQL_DELIMITER, input, 13 );
	piece( additional_index_yn, SQL_DELIMITER, input, 14 );
	piece( lookup_required_yn, SQL_DELIMITER, input, 15 );
	piece( insert_required_yn, SQL_DELIMITER, input, 16 );
	piece( lookup_histogram_output_yn, SQL_DELIMITER, input, 17 );
	piece( lookup_time_chart_output_yn, SQL_DELIMITER, input, 18 );
	piece( appaserver_yn, SQL_DELIMITER, input, 19 );

	return attribute_new_attribute(
				strdup( folder_name ),
				strdup( attribute_name ),
				strdup( datatype ),
				atoi( width ),
				atoi( primary_key_index ),
				atoi( display_order ),
				atoi( float_decimal_places ),
				(boolean) (*omit_insert_yn == 'y'),
				(boolean) (*omit_insert_prompt_yn == 'y'),
				(boolean) (*omit_update_yn == 'y'),
				strdup( hint_message ),
				strdup( post_change_javascript ),
				strdup( on_focus_javascript_function ),
				(boolean) (*additional_unique_index_yn == 'y'),
				(boolean) (*additional_index_yn == 'y'),
				(boolean) (*lookup_histogram_output_yn == 'y'),
				(boolean) (*lookup_time_chart_output_yn == 'y'),
				(boolean) (*lookup_required_yn == 'y'),
				(boolean) (*insert_required_yn == 'y'),
				(boolean) (*appaserver_yn == 'y') );
}

LIST *attribute_primary_name_list(
			LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *primary_attribute_name_list;

	if ( !attribute_list || !list_rewind( attribute_list ) )
		return (LIST *)0;

	primary_attribute_name_list = list_new_list();

	do {
		attribute = list_get( attribute_list );

		if ( attribute->primary_key_index )
		{
			list_append_string(
					primary_attribute_name_list,
					attribute->attribute_name );
		}
	} while( list_next( attribute_list ) );

	return primary_attribute_name_list;
}
