/* ---------------------------------------------------	*/
/* src_benthic/braun_blanque.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "query.h"
#include "braun_blanque.h"
#include "benthic_library.h"
#include "piece.h"
#include "appaserver_library.h"

BRAUN_BLANQUE *braun_blanque_new(	char *application_name )
{
	BRAUN_BLANQUE *braun_blanque;

	if ( ! ( braun_blanque = calloc( 1, sizeof( BRAUN_BLANQUE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	braun_blanque->input_record = braun_blanque_input_record_new();

	braun_blanque->vegetation_group_list =
		braun_blanque_get_vegetation_group_list( application_name );

	braun_blanque->vegetation_list =
		braun_blanque_get_vegetation_list(
			application_name,
			braun_blanque->vegetation_group_list );

	braun_blanque->vegetation_array =
		braun_blanque_vegetation_array_new(
			braun_blanque->vegetation_list );

	braun_blanque->master_substrate_list =
		braun_blanque_get_substrate_list( application_name );

	braun_blanque->output_record =
		braun_blanque_output_record_new(
			list_length( braun_blanque->vegetation_list ),
			list_length( braun_blanque->vegetation_group_list ) );

	return braun_blanque;
}

VALUE *braun_blanque_value_new(	double value_double )
{
	VALUE *value;

	if ( ! ( value = calloc( 1, sizeof( VALUE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	value->value = value_double;

	return value;
}

INPUT_RECORD *braun_blanque_input_record_new( void )
{
	INPUT_RECORD *input_record;

	if ( ! ( input_record = calloc( 1, sizeof( INPUT_RECORD ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return input_record;
}

OUTPUT_RECORD *braun_blanque_output_record_new(
				int length_vegetation_list,
				int length_vegetation_group_list )
{
	OUTPUT_RECORD *output_record;

	if ( ! ( output_record = calloc( 1, sizeof( OUTPUT_RECORD ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	output_record->value_array =
		braun_blanque_value_array_new(
			length_vegetation_list );

	output_record->group_value_array =
		braun_blanque_group_value_array_new(
			length_vegetation_group_list );

	output_record->substrate_list = list_new();

	return output_record;
}

SUBSTRATE *braun_blanque_substrate_new(
					char *substrate_name,
					char *substrate_code )
{
	SUBSTRATE *substrate;

	if ( ! ( substrate = calloc( 1, sizeof( SUBSTRATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	substrate->substrate = substrate_name;
	substrate->substrate_code = substrate_code;

	return substrate;
}

VEGETATION_GROUP *braun_blanque_vegetation_group_new(
					char *vegetation_group_name,
					char *heading_label )
{
	VEGETATION_GROUP *vegetation_group;

	if ( ! ( vegetation_group = calloc( 1, sizeof( VEGETATION_GROUP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	vegetation_group->vegetation_group_name = vegetation_group_name;
	vegetation_group->heading_label = heading_label;

	return vegetation_group;
}

VEGETATION_GROUP_VALUE *braun_blanque_vegetation_group_value_new(
						void )
{
	VEGETATION_GROUP_VALUE *vegetation_group_value;

	if ( ! ( vegetation_group_value =
			calloc( 1, sizeof( VEGETATION_GROUP_VALUE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return vegetation_group_value;
}

VALUE **braun_blanque_value_array_new( int length_vegetation_list )
{
	VALUE **value_array;

	if ( ! ( value_array =
			calloc(
			length_vegetation_list,
			sizeof( VALUE * ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return value_array;
}

VEGETATION_GROUP_VALUE **braun_blanque_group_value_array_new(
				int length_vegetation_group_list )
{
	VEGETATION_GROUP_VALUE **group_value_array;

	if ( ! ( group_value_array =
			calloc(
			length_vegetation_group_list,
			sizeof( VEGETATION_GROUP_VALUE * ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return group_value_array;
}

char *braun_blanque_group_heading_label_list_display(
				LIST *vegetation_group_list )
{
	VEGETATION_GROUP *vegetation_group;
	char heading_label_list_string[ 65536 ];
	char *ptr = heading_label_list_string;

	if ( !list_rewind( vegetation_group_list ) ) return (char *)0;

	do {
		vegetation_group = list_get_pointer( vegetation_group_list );

		if ( ptr != heading_label_list_string )
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", vegetation_group->heading_label );
	} while( list_next( vegetation_group_list ) );
	*ptr = '\0';
	return strdup( heading_label_list_string );

}

char *braun_blanque_heading_label_list_display(
				LIST *vegetation_list )
{
	VEGETATION *vegetation;
	char heading_label_list_string[ 65536 ];
	char *ptr = heading_label_list_string;

	if ( !list_rewind( vegetation_list ) ) return (char *)0;

	do {
		vegetation = list_get_pointer( vegetation_list );

		if ( ptr != heading_label_list_string )
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", vegetation->heading_label );
	} while( list_next( vegetation_list ) );
	*ptr = '\0';
	return strdup( heading_label_list_string );

}

LIST *braun_blanque_get_total_vegetation_list(
				char *application_name )
{
	LIST *vegetation_list;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char vegetation_name[ 256 ];
	char *vegetation_name_pointer;
	FILE *input_pipe;
	char *select;
	char *folder;
	char *where;
	char *order;

	vegetation_list = list_new();

	select = "vegetation_name";
	folder = "vegetation_species";
	where = "vegetation_group = 'Aggregate Habitat'";
	order = "vegetation_name";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=%s			",
		 application_name,
		 select,
		 folder,
		 where,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	vegetation_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		vegetation_name_pointer = strdup( vegetation_name );

		braun_blanque_append_vegetation_list(
			vegetation_list,
			vegetation_name_pointer,
			'n' /* is_height_yn */,
			vegetation_name_pointer /* heading_label */,
			(VEGETATION_GROUP *)0 );
	}

	pclose( input_pipe );
	return vegetation_list;

}

LIST *braun_blanque_get_vegetation_list(char *application_name,
					LIST *vegetation_group_list )
{
	LIST *vegetation_list;

	vegetation_list =
		braun_blanque_get_total_vegetation_list(
			application_name );

	if ( !list_length( vegetation_list ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot get total vegetation_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_append_list(	vegetation_list,
				braun_blanque_get_seagrass_vegetation_list(
					application_name,
					vegetation_group_list ) );

	list_append_list(	vegetation_list,
				braun_blanque_get_non_seagrass_vegetation_list(
					application_name,
					vegetation_group_list ) );

	return vegetation_list;

}

SUBSTRATE *braun_blanque_substrate_seek(LIST *master_substrate_list,
					char *substrate_name )
{
	SUBSTRATE *substrate;

	if ( !list_rewind( master_substrate_list ) )
		return (SUBSTRATE *)0;

	do {
		substrate = list_get_pointer( master_substrate_list );

		if ( timlib_strcmp(	substrate->substrate,
					substrate_name ) == 0 )
		{
			return substrate;
		}
	} while( list_next( master_substrate_list ) );
	return (SUBSTRATE *)0;
}

LIST *braun_blanque_get_substrate_list( char *application_name )
{
	LIST *substrate_list;
	char input_buffer[ 128 ];
	char substrate_name[ 128 ];
	char substrate_code[ 128 ];
	FILE *input_pipe;
	char sys_string[ 1024 ];
	SUBSTRATE *substrate;
	char *select = "substrate,substrate_code";

	substrate_list = list_new();

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=substrate		",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{

		piece(	substrate_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	substrate_code,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		substrate = braun_blanque_substrate_new(
					strdup( substrate_name ),
					strdup( substrate_code ) );

		list_append_pointer( substrate_list, substrate );
	}

	pclose( input_pipe );
	return substrate_list;

}

LIST *braun_blanque_get_vegetation_group_list( char *application_name )
{
	LIST *vegetation_group_list;
	char input_buffer[ 128 ];
	char vegetation_group_name[ 128 ];
	char vegetation_group_code[ 128 ];
	FILE *input_pipe;
	char sys_string[ 1024 ];
	VEGETATION_GROUP *vegetation_group;
	char *select = "vegetation_group,vegetation_group_code";
	char *order = "display_order";

	vegetation_group_list = list_new();

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=vegetation_group		"
		 "			order=%s			",
		 application_name,
		 select,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{

		piece(	vegetation_group_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	vegetation_group_code,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		vegetation_group = braun_blanque_vegetation_group_new(
					strdup( vegetation_group_name ),
					strdup( vegetation_group_code ) );

		list_append_pointer( vegetation_group_list, vegetation_group );
	}

	pclose( input_pipe );
	return vegetation_group_list;

}

LIST *braun_blanque_get_seagrass_vegetation_list(
				char *application_name,
				LIST *vegetation_group_list )
{
	LIST *vegetation_list;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char vegetation_name[ 256 ];
	char *vegetation_name_pointer;
	char vegetation_species_code[ 256 ];
	char *vegetation_species_code_pointer;
	FILE *input_pipe;
	VEGETATION_GROUP *group;
	char *select;
	char *folder;
	char *where;
	char *order;
	char heading_label[ 128 ];

	vegetation_list = list_new();

	group = braun_blanque_vegetation_group_seek(
			BRAUN_BLANQUE_SEAGRASS,
			vegetation_group_list );

	select = "vegetation_name,vegetation_species_code";
	folder = "vegetation_species";
	where = "vegetation_group = 'seagrass'";
	order = "vegetation_name";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=%s			",
		 application_name,
		 select,
		 folder,
		 where,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	vegetation_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		vegetation_name_pointer = strdup( vegetation_name );

		piece(	vegetation_species_code,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		vegetation_species_code_pointer =
			strdup( vegetation_species_code );

		sprintf( heading_label,
			 "%scanopy",
			 vegetation_species_code_pointer );
		braun_blanque_append_vegetation_list(
			vegetation_list,
			vegetation_name_pointer,
			'y' /* is_height_yn */,
			strdup( heading_label ),
			group );

		sprintf( heading_label,
			 "%s",
			 vegetation_name_pointer );
		braun_blanque_append_vegetation_list(
			vegetation_list,
			vegetation_name_pointer,
			'n' /* is_height_yn */,
			strdup( heading_label ),
			group );

	}

	pclose( input_pipe );
	return vegetation_list;

}

LIST *braun_blanque_get_non_seagrass_vegetation_list(
			char *application_name,
			LIST *vegetation_group_list )
{
	LIST *vegetation_list;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char vegetation_name[ 256 ];
	char vegetation_group[ 256 ];
	FILE *input_pipe;
	VEGETATION_GROUP *group;
	char *select;
	char *folder;
	char *where;
	char *order;

	vegetation_list = list_new();

	select = "vegetation_name,vegetation_group";
	folder = "vegetation_species";
	where = "vegetation_group not in ('seagrass', 'Aggregate Habitat')";
	order = "vegetation_name";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=%s			",
		 application_name,
		 select,
		 folder,
		 where,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	vegetation_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	vegetation_group,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		group = braun_blanque_vegetation_group_seek(
				vegetation_group,
				vegetation_group_list );

		braun_blanque_append_vegetation_list(
			vegetation_list,
			strdup( vegetation_name ),
			'n' /* is_height_yn */,
			strdup( vegetation_name ) /* heading_label */,
			group );
	}

	pclose( input_pipe );
	return vegetation_list;

}

VEGETATION *braun_blanque_vegetation_new(
				char *vegetation_name,
				char is_height_yn,
				char *heading_label,
				VEGETATION_GROUP *group )
{
	VEGETATION *vegetation;

	if ( ! ( vegetation = calloc( 1, sizeof( VEGETATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	vegetation->vegetation_name = vegetation_name;
	vegetation->is_height_yn = is_height_yn;
	vegetation->heading_label = heading_label;
	vegetation->vegetation_group = group;
	return vegetation;

}

void braun_blanque_append_vegetation_list(
				LIST *vegetation_list,
				char *vegetation_name,
				char is_height_yn,
				char *heading_label,
				VEGETATION_GROUP *group )
{
	VEGETATION *vegetation;

	vegetation = braun_blanque_vegetation_new(
				vegetation_name,
				is_height_yn,
				heading_label,
				group );

	list_append_pointer( vegetation_list, vegetation );

}

VEGETATION *braun_blanque_append_vegetation_seek(
				LIST *vegetation_list,
				char *vegetation_name,
				char is_height_yn )
{
	VEGETATION *vegetation;

	if ( !list_rewind( vegetation_list ) ) return (VEGETATION *)0;

	do {
		vegetation = list_get_pointer( vegetation_list );

		if ( timlib_strcmp(	vegetation->vegetation_name,
					vegetation_name ) == 0
		&&   braun_blanque_is_height_yn_match(
					is_height_yn,
					vegetation->is_height_yn ) )
		{
			return vegetation;
		}
	} while( list_next( vegetation_list ) );
	return (VEGETATION *)0;
}

boolean braun_blanque_vegetation_exists(
				LIST *vegetation_list,
				char *vegetation_name,
				char is_height_yn )
{
	return (boolean)braun_blanque_append_vegetation_seek(
				vegetation_list,
				vegetation_name,
				is_height_yn );

}

boolean braun_blanque_set_input_buffer(
				FILE *output_file,
				OUTPUT_RECORD *output_record,
				INPUT_RECORD *input_record,
				char *input_buffer,
				LIST *vegetation_list,
				LIST *vegetation_group_list,
				LIST *master_substrate_list,
				VEGETATION **vegetation_array,
				boolean avg_quads_boolean )
{
	char piece_buffer[ 256 ];
	static char old_key[ 256 ] = {0};
	char new_key[ 256 ] = {0};
	int piece_offset;

	/* Colletion name */
	/* -------------- */
	if ( !piece( piece_buffer, ',', input_buffer, 2 ) )
	{
		fprintf( stderr,
			"Error in %s/%s()/%d: cannot piece(',',2) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		timlib_strcpy(	input_record->collection_name,
				piece_buffer,
				BRAUN_BLANQUE_STRING_SIZE );
	}

	/* Location name */
	/* ------------- */
	if ( !piece( piece_buffer, ',', input_buffer, 3 ) )
	{
		fprintf( stderr,
			"Error in %s/%s()/%d: cannot piece(',',3) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		timlib_strcpy(	input_record->location,
				piece_buffer,
				BRAUN_BLANQUE_STRING_SIZE );
	}

	/* Site number */
	/* ----------- */
	if ( !piece( piece_buffer, ',', input_buffer, 4 ) )
	{
		fprintf( stderr,
			"Error in %s/%s()/%d: cannot piece(',',4) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		input_record->site_number = atoi( piece_buffer );
	}

	if ( !avg_quads_boolean )
	{
		/* Quad */
		/* ---- */
		if ( !piece( piece_buffer, ',', input_buffer, 5 ) )
		{
			fprintf( stderr,
			"Error in %s/%s()/%d: cannot piece(',',5) with (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			exit( 1 );
		}
		else
		{
			input_record->quad = atoi( piece_buffer );
		}
	}

	if ( !avg_quads_boolean )
		piece_offset = 6;
	else
		piece_offset = 5;

	/* Latitude */
	/* -------- */
	if ( !piece( piece_buffer, ',', input_buffer, piece_offset ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot piece(',',%d) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 piece_offset,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		timlib_strcpy(	input_record->actual_latitude,
				piece_buffer,
				BRAUN_BLANQUE_STRING_SIZE );
	}

	if ( !avg_quads_boolean )
		piece_offset = 7;
	else
		piece_offset = 6;

	/* Longitude */
	/* --------- */
	if ( !piece( piece_buffer, ',', input_buffer, piece_offset ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot piece(',',%d) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 piece_offset,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		timlib_strcpy(	input_record->actual_longitude,
				piece_buffer,
				BRAUN_BLANQUE_STRING_SIZE );
	}

	/* If first time */
	/* ------------- */
	if ( !*old_key )
	{
		strcpy( old_key,
			braun_blanque_get_key(
				input_record->collection_name,
				input_record->location,
				input_record->site_number,
				input_record->quad ) );

		timlib_strcpy( new_key, old_key, 256 );

		/* Start a new line */
		/* ---------------- */
		braun_blanque_set_output_anchor(
				output_record,
				input_record->collection_name,
				input_record->location,
				input_record->site_number,
				input_record->quad );
	}
	else
	{
		strcpy( new_key,
			braun_blanque_get_key(
				input_record->collection_name,
				input_record->location,
				input_record->site_number,
				input_record->quad ) );
	}

	/* If a change in the anchor. */
	/* -------------------------- */
	if ( timlib_strcmp( old_key, new_key ) != 0 )
	{
		/* Output the line */
		/* --------------- */
		braun_blanque_output_record(
					output_file,
					output_record,
					list_length( vegetation_list ),
					list_length( vegetation_group_list ),
					vegetation_array,
					avg_quads_boolean );

		braun_blanque_clear_value_array(
				output_record->value_array,
				list_length( vegetation_list ) );

		braun_blanque_clear_group_value_array(
				output_record->group_value_array,
				list_length( vegetation_group_list ) );

		output_record->substrate_list =
			braun_blanque_clear_substrate_list(
				output_record->substrate_list );

		braun_blanque_clear_aggregate_values( output_record );

		/* Start a new line */
		/* ---------------- */
		braun_blanque_set_output_anchor(
				output_record,
				input_record->collection_name,
				input_record->location,
				input_record->site_number,
				input_record->quad );

		timlib_strcpy( old_key, new_key, 256 );
	}

	/* Collection number */
	/* ----------------- */
	piece( piece_buffer, ',', input_buffer, 0 );
	if ( ! ( input_record->collection_number = atoi( piece_buffer ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: collection_number is zero.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Location number */
	/* --------------- */
	if ( !piece( piece_buffer, ',', input_buffer, 1 ) )
	{
		fprintf( stderr,
			"Error in %s/%s()/%d: cannot piece(',',1) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		input_record->location_number = atoi( piece_buffer );
	}

	input_record->season =
		benthic_library_get_season(
			input_record->collection_name );

	if ( !avg_quads_boolean )
		piece_offset = 8;
	else
		piece_offset = 7;

	/* Region code */
	/* ----------- */
	if ( !piece( piece_buffer, ',', input_buffer, piece_offset ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot piece(',',%d) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 piece_offset,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		timlib_strcpy(	input_record->region_code,
				piece_buffer,
				BRAUN_BLANQUE_STRING_SIZE );
	}

	if ( !avg_quads_boolean )
		piece_offset = 9;
	else
		piece_offset = 8;

	/* Pink assessment area */
	/* -------------------- */
	if ( !piece( piece_buffer, ',', input_buffer, piece_offset ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot piece(',',%d) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 piece_offset,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		input_record->pink_assessment_area = atoi( piece_buffer );
	}

	if ( !avg_quads_boolean )
		piece_offset = 10;
	else
		piece_offset = 9;

	/* Vegetation name */
	/* --------------- */
	if ( !piece( piece_buffer, ',', input_buffer, piece_offset ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot piece(',',%d) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 piece_offset,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		timlib_strcpy(	input_record->vegetation_name,
				piece_buffer,
				BRAUN_BLANQUE_STRING_SIZE );
	}

/*
12,01,Fall10,north_biscayne_bay,25.8,-80.5,bb,0,01,01,substrate,mud
*/

	/* If substrate record */
	/* ------------------- */
	if ( strcmp(	input_record->vegetation_name,
			BRAUN_BLANQUE_SUBSTRATE_KEY ) == 0 )
	{
		SUBSTRATE *substrate;
		char piece_buffer[ 256 ];

		if ( !avg_quads_boolean )
			piece_offset = 11;
		else
			piece_offset = 10;

		/* Substrate name */
		/* --------------- */
		if ( !piece( piece_buffer, ',', input_buffer, piece_offset ) )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: cannot piece(',',%d) with (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
				piece_offset,
			 	input_buffer );
			exit( 1 );
		}

		if ( ( substrate = braun_blanque_substrate_seek(
					master_substrate_list,
					piece_buffer /* substrate_name */ ) ) )
		{
			list_append_pointer(	output_record->substrate_list,
						substrate );
		}

		braun_blanque_set_output_record(
			output_record,
			input_record,
			(LIST *)0 /* vegetation_list */,
			(LIST *)0 /* vegetation_group_list */ );
		return 1;
	}

	if ( !avg_quads_boolean )
		piece_offset = 11;
	else
		piece_offset = 10;

	/* Braun Blanque Value */
	/* ------------------- */
	if ( !piece( piece_buffer, ',', input_buffer, piece_offset ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: cannot piece(',',%d) with (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 piece_offset,
			 input_buffer );
		exit( 1 );
	}
	else
	{
		input_record->value = atof( piece_buffer );
/*
fprintf( stderr, "\n%s/%s()/%d: got input_record->value = (%.1lf)\n",
__FILE__,
__FUNCTION__,
__LINE__,
input_record->value );
*/

		input_record->is_height_yn = 'n';

		braun_blanque_set_output_record(
			output_record,
			input_record,
			vegetation_list,
			vegetation_group_list );
	}
	
/*
12,01,Fall10,north_biscayne_bay,25.4,-80.5,bb,0,01,01,Syringodium,4 = 50-75% cover present,92
*/

	if ( !avg_quads_boolean )
		piece_offset = 12;
	else
		piece_offset = 11;

	/* Vegetation height */
	/* ----------------- */
	if ( piece( piece_buffer, ',', input_buffer, piece_offset ) )
	{
		double vegetation_height;

		vegetation_height = atof( piece_buffer );

		if ( vegetation_height )
		{
			/* Set the vegetation height */
			/* ------------------------- */
			input_record->value = vegetation_height;
			input_record->is_height_yn = 'y';

			braun_blanque_set_output_record(
				output_record,
				input_record,
				vegetation_list,
				vegetation_group_list );
		}
	}

	return 1;
}

void braun_blanque_set_output_record(
		OUTPUT_RECORD *output_record,
		INPUT_RECORD *input_record,
		LIST *vegetation_list,
		LIST *vegetation_group_list )
{
	char *vegetation_group_name;

	output_record->collection_number = input_record->collection_number;
	output_record->location_number = input_record->location_number;

	timlib_strcpy(	output_record->actual_latitude,
			input_record->actual_latitude,
			BRAUN_BLANQUE_STRING_SIZE );

	timlib_strcpy(	output_record->actual_longitude,
			input_record->actual_longitude,
			BRAUN_BLANQUE_STRING_SIZE );

	timlib_strcpy(	output_record->region_code,
			input_record->region_code,
			BRAUN_BLANQUE_STRING_SIZE );

	output_record->pink_assessment_area =
		input_record->pink_assessment_area;

	output_record->season = input_record->season;

	if ( list_length( vegetation_list ) )
	{
		braun_blanque_set_value_array(
			output_record->value_array,
			input_record->vegetation_name,
			input_record->value,
			vegetation_list,
			input_record->is_height_yn );

		if ( input_record->is_height_yn == 'y' )
		{
			output_record->max_height =
				braun_blanque_get_max_avg_height(
					&output_record->average_height,
					output_record->value_array,
					vegetation_list,
					BRAUN_BLANQUE_SEAGRASS );
		}
	}

	if ( input_record->is_height_yn != 'y'
	&&   list_length( vegetation_group_list ) )
	{
		vegetation_group_name =
			braun_blanque_get_vegetation_group_name(
				vegetation_list,
				input_record->vegetation_name );

		if ( vegetation_group_name )
		{
			braun_blanque_set_group_value_array(
				output_record->group_value_array,
				vegetation_group_name,
				input_record->value,
				vegetation_group_list );
		}
	}

}

char *braun_blanque_get_key(	char *collection_name,
				char *location,
				int site_number,
				int quad )
{
	static char key[ 256 ];

	sprintf(	key,
			"%s|%s|%d|%d",
			collection_name,
			location,
			site_number,
			quad );
	return key;
}

void braun_blanque_set_output_anchor(
				OUTPUT_RECORD *output_record,
				char *collection_name,
				char *location,
				int site_number,
				int quad )
{
	timlib_strcpy(	output_record->collection_name,
			collection_name,
			BRAUN_BLANQUE_STRING_SIZE );

	timlib_strcpy(	output_record->location,
			location,
			BRAUN_BLANQUE_STRING_SIZE );

	output_record->site_number = site_number;
	output_record->quad = quad;

}

LIST *braun_blanque_clear_substrate_list( LIST *substrate_list )
{
	if ( substrate_list ) list_free_container( substrate_list );
	return list_new();
}

void braun_blanque_clear_group_value_array(
				VEGETATION_GROUP_VALUE **group_value_array,
				int length_vegetation_group_list )
{
	int i;
	VEGETATION_GROUP_VALUE *group_value;

	for( i = 0; i < length_vegetation_group_list; i++ )
	{
		if ( group_value_array[ i ] )
		{
			group_value = group_value_array[ i ];
			group_value_array[ i ] = (VEGETATION_GROUP_VALUE *)0;
		}
	}

}

void braun_blanque_clear_value_array(
				VALUE **value_array,
				int length_vegetation_list )
{
	int i;
	VALUE *value;

	for( i = 0; i < length_vegetation_list; i++ )
	{
		if ( value_array[ i ] )
		{
			value = value_array[ i ];
			value_array[ i ] = (VALUE *)0;
		}
	}

}

void braun_blanque_set_group_value_array(
				VEGETATION_GROUP_VALUE **group_value_array,
				char *vegetation_group_name,
				double value_double,
				LIST *vegetation_group_list )
{
	int output_record_value_index;
	VEGETATION_GROUP_VALUE *vegetation_group_value;

	if ( ( output_record_value_index =
		braun_blanque_get_vegetation_group_index(
			vegetation_group_name,
			vegetation_group_list ) ) == -1 )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot find (%s) in vegetation_group_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 vegetation_group_name );
		exit( 1 );
	}

	if ( !group_value_array[ output_record_value_index ] )
	{
		vegetation_group_value =
			braun_blanque_vegetation_group_value_new();
		group_value_array[ output_record_value_index ] =
			vegetation_group_value;
	}

	group_value_array[ output_record_value_index ]->value_sum +=
		value_double;
	group_value_array[ output_record_value_index ]->count++;

}

void braun_blanque_set_value_array(
			VALUE **value_array,
			char *vegetation_name,
			double value_double,
			LIST *vegetation_list,
			char is_height_yn )
{
	int output_record_value_index;
	VALUE *value;

	if ( ( output_record_value_index =
		braun_blanque_get_output_record_value_index(
			vegetation_name,
			vegetation_list,
			is_height_yn ) ) == -1 )
	{
		return;
	}

	value = braun_blanque_value_new( value_double );
	value_array[ output_record_value_index ] = value;

}

VEGETATION_GROUP *braun_blanque_vegetation_group_seek(
			char *vegetation_group_name,
			LIST *vegetation_group_list )
{
	VEGETATION_GROUP *vegetation_group;

	if ( list_rewind( vegetation_group_list ) )
	{
		do {
			vegetation_group =
				list_get_pointer(
					vegetation_group_list );

			if ( timlib_strcmp(	vegetation_group->
							vegetation_group_name,
						vegetation_group_name ) == 0 )
			{
				return vegetation_group;
			}
		} while( list_next( vegetation_group_list ) );
	}
	fprintf( stderr,
		 "ERROR in %s/%s()/%d: cannot find vegetation_group = (%s).\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__,
		 vegetation_group_name );
	exit( 1 );
}

int braun_blanque_get_vegetation_group_index(
			char *vegetation_group_name,
			LIST *vegetation_group_list )
{
	int value_index = -1;
	VEGETATION_GROUP *vegetation_group;

	if ( list_rewind( vegetation_group_list ) )
	{
		do {
			vegetation_group =
				list_get_pointer(
					vegetation_group_list );

			value_index++;

			if ( timlib_strcmp(	vegetation_group->
							vegetation_group_name,
						vegetation_group_name ) == 0 )
			{
				return value_index;
			}
		} while( list_next( vegetation_group_list ) );
	}
	return -1;
}
 
int braun_blanque_get_output_record_value_index(
			char *vegetation_name,
			LIST *vegetation_list,
			char is_height_yn )
{
	int value_index = -1;
	VEGETATION *vegetation;

	if ( list_rewind( vegetation_list ) )
	{
		do {
			vegetation = list_get_pointer( vegetation_list );

			value_index++;

			if ( timlib_strcmp(	vegetation->vegetation_name,
						vegetation_name ) == 0
			&&   vegetation->is_height_yn == is_height_yn )
			{
				return value_index;
			}
		} while( list_next( vegetation_list ) );
	}
	return -1;
}

boolean braun_blanque_is_height_yn_match(	char is_height_yn1,
						char is_height_yn2 )
{
	if ( is_height_yn1 == 'y' && is_height_yn2 == 'y' )
		return 1;
	else
	if ( is_height_yn1 == 'y' )
		return 0;
	else
	if ( is_height_yn2 == 'y' )
		return 0;
	else
		return 1;
}

char *braun_blanque_or_sequence_get_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list )
{
	LIST *column_name_list;
	char *sampling_point_table_name;
	char column_name[ 128 ];
	QUERY_OR_SEQUENCE *query_or_sequence;

	column_name_list = list_new();

	sampling_point_table_name =
		get_table_name(
			application_name, "sampling_point" );

	sprintf(	column_name,
			"%s.collection_name",
			sampling_point_table_name );

	list_append_pointer( column_name_list, strdup( column_name ) );

	sprintf(	column_name,
			"%s.project",
			sampling_point_table_name );

	list_append_pointer( column_name_list, strdup( column_name ) );

	query_or_sequence = query_or_sequence_new( column_name_list );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			collection_list );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			project_list );

	return query_or_sequence_where_clause(
				query_or_sequence->attribute_name_list,
				query_or_sequence->data_list_list,
				0 /* not with_and_prefix */ );
}

char *braun_blanque_get_substrate_display(
				LIST *substrate_list )
{
	SUBSTRATE *substrate;
	static char display[ 1024 ];
	char *ptr = display;

	*display = '\0';

	if ( !list_rewind( substrate_list ) ) return "";

	do {
		substrate = list_get_pointer( substrate_list );

		if ( ptr != display ) ptr += sprintf( ptr, "/" );
		ptr += sprintf( ptr, "%s", substrate->substrate_code );

	} while( list_next( substrate_list ) );
	*ptr = '\0';
	return display;
}

char *braun_blanque_vegetation_list_display(
					LIST *vegetation_list )
{
	VEGETATION *vegetation;
	static char display[ 65536 ];
	char *ptr = display;

	*ptr = '\0';

	if ( !list_rewind( vegetation_list ) ) return display;

	do {
		vegetation = list_get_pointer( vegetation_list );

		ptr += sprintf( ptr,
		"\nVegetation name: %s\nIs height yn: %c\nHeading label: %s\n",
				vegetation->vegetation_name,
				vegetation->is_height_yn,
				vegetation->heading_label );

		ptr += sprintf( ptr,
				"%s\n",
				braun_blanque_vegetation_group_display(
					vegetation->
						vegetation_group ) );
	} while( list_next( vegetation_list ) );

	return display;
}

char *braun_blanque_vegetation_group_display(
				VEGETATION_GROUP *vegetation_group )
{
	char display[ 1024 ];

	*display = '\0';

	if ( vegetation_group )
	{
		sprintf( display,
		 "\tVegetation group name: %s\n\tHeading: %s\n",
		 	vegetation_group->vegetation_group_name,
		 	vegetation_group->heading_label );
	}
	return strdup( display );
}

VEGETATION *braun_blanque_vegetation_seek(
					LIST *vegetation_list,
					char *vegetation_name )
{
	VEGETATION *vegetation;

	if ( !list_rewind( vegetation_list ) ) return (VEGETATION *)0;

	do {
		vegetation = list_get_pointer( vegetation_list );
		if ( timlib_strcmp(	vegetation->vegetation_name,
					vegetation_name ) == 0 )
		{
			return vegetation;
		}
	} while( list_next( vegetation_list ) );

	return (VEGETATION *)0;

}

char *braun_blanque_get_vegetation_group_name(
					LIST *vegetation_list,
					char *vegetation_name )
{
	VEGETATION *vegetation;

	if ( ! ( vegetation = braun_blanque_vegetation_seek(
					vegetation_list,
					vegetation_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 vegetation_name );
		exit( 1 );
	}

	if ( !vegetation->vegetation_group )
	{
		return (char *)0;
	}

	return vegetation->vegetation_group->vegetation_group_name;

}

double braun_blanque_get_max_avg_height(
			double *average_height,
			VALUE **value_array,
			LIST *vegetation_list,
			char *seagrass_group )
{
	VEGETATION *vegetation;
	VALUE *value;
	double max_height = 0.0;
	int value_offset = 0;
	double total_height = 0.0;
	int count = 0;

	*average_height = 0.0;

	if ( !list_rewind( vegetation_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty vegetation_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		vegetation = list_get_pointer( vegetation_list );

		if ( vegetation->is_height_yn == 'y'
		&&   vegetation->vegetation_group
		&&   timlib_strcmp(	vegetation->
						vegetation_group->
						vegetation_group_name,
					seagrass_group ) == 0 )
		{
			value = value_array[ value_offset ];

			if ( value && ( value->value > max_height ) )
			{
				max_height = value->value;
			}

			if ( value && value->value )
			{
				count++;
				total_height += value->value;
			}

		}
		value_offset++;
	} while( list_next( vegetation_list ) );

	if ( count ) *average_height = total_height / (double)count;

	return max_height;
}

VEGETATION **braun_blanque_vegetation_array_new(
			LIST *vegetation_list )
{
	VEGETATION **vegetation_array;
	VEGETATION *vegetation;
	int i = 0;

	if ( !list_length( vegetation_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty vegetation_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( vegetation_array =
			calloc(
			list_length( vegetation_list ),
			sizeof( VEGETATION * ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_rewind( vegetation_list );

	do {
		vegetation = list_get_pointer( vegetation_list );
		vegetation_array[ i++ ] = vegetation;
	} while( list_next( vegetation_list ) );
	return vegetation_array;
}

void braun_blanque_output_record(	FILE *output_file,
					OUTPUT_RECORD *output_record,
					int length_vegetation_list,
					int length_vegetation_group_list,
					VEGETATION **vegetation_array,
					boolean avg_quads_boolean )
{
	VALUE *value;
	VALUE **value_array;
	VEGETATION_GROUP_VALUE **group_value_array;
	VEGETATION_GROUP_VALUE *vegetation_group_value;
	int i;
	char max_height_string[ 16 ];
	char average_height_string[ 16 ];
	VEGETATION *vegetation;
	VEGETATION *next_vegetation;
	VALUE *next_value;

	value_array = output_record->value_array;
	group_value_array = output_record->group_value_array;

	if ( output_record->max_height )
	{
		sprintf(	max_height_string,
				"%.1lf",
				output_record->max_height );
	}
	else
	{
		strcpy( max_height_string, "0" );
	}

	if ( output_record->average_height )
	{
		sprintf(	average_height_string,
				"%.1lf",
				output_record->average_height );
	}
	else
	{
		strcpy( average_height_string, "0" );
	}

	if ( !avg_quads_boolean )
	{
		fprintf( output_file,
			 "%d,%d,%s,%s,%s,%s,%s,%d,%d,%d,%d,%s,%s,%s",
			 output_record->collection_number,
			 output_record->location_number,
			 output_record->collection_name,
			 output_record->location,
			 output_record->actual_latitude,
			 output_record->actual_longitude,
			 output_record->region_code,
			 output_record->pink_assessment_area,
			 output_record->site_number,
			 output_record->quad,
			 output_record->season,
			 braun_blanque_get_substrate_display(
					output_record->substrate_list ),
			 max_height_string,
			 average_height_string );
	}
	else
	{
		fprintf( output_file,
			 "%d,%d,%s,%s,%s,%s,%s,%d,%d,%d,%s,%s,%s",
			 output_record->collection_number,
			 output_record->location_number,
			 output_record->collection_name,
			 output_record->location,
			 output_record->actual_latitude,
			 output_record->actual_longitude,
			 output_record->region_code,
			 output_record->pink_assessment_area,
			 output_record->site_number,
			 output_record->season,
			 braun_blanque_get_substrate_display(
					output_record->substrate_list ),
			 max_height_string,
			 average_height_string );
	}

	for( i = 0; i < length_vegetation_group_list; i++ )
	{
		vegetation_group_value = group_value_array[ i ];

		if ( vegetation_group_value )
		{
			if ( !vegetation_group_value->count )
			{
				fprintf( output_file, ",0" );
			}
			else
			{
				fprintf(output_file,
					",%.1lf",
					vegetation_group_value->value_sum /
					(double)
					vegetation_group_value->count );
			}
		}
		else
		{
			fprintf( output_file, ",0" );
		}
	}

	for( i = 0; i < length_vegetation_list; i++ )
	{
		value = value_array[ i ];
		if ( i == ( length_vegetation_list - 1 ) )
			next_value = (VALUE *)0;
		else
			next_value = value_array[ i + 1 ];

		if ( value )
		{
			fprintf( output_file, ",%.1lf", value->value );
		}
		else
		{
			if ( !vegetation_array[ i ] )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: empty vegetation_array[ %d ].\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 i );
				exit( 1 );
			}

			vegetation = vegetation_array[ i ];

			if ( i == ( length_vegetation_list - 1 ) )
				next_vegetation = (VEGETATION *)0;
			else
				next_vegetation = vegetation_array[ i + 1 ];

			/* The total columns don't have groups */
			/* ----------------------------------- */
			if ( !vegetation->vegetation_group )
			{
				fprintf( output_file, ",0" );
			}
			else
			/* --------------------------------------- */
			/* Check the for missing seagrass heights. */
			/* --------------------------------------- */
			if ( timlib_strcmp(	vegetation->
						     vegetation_group->
						     vegetation_group_name,
						"seagrass" ) == 0
			&&   vegetation->is_height_yn == 'y'
			&&   next_vegetation
			&&   next_value )
			{
				if ( timlib_strcmp(
					next_vegetation->
						vegetation_name,
					vegetation->
						vegetation_name ) == 0
				&&   next_vegetation->is_height_yn != 'y'
				&&   next_value->value )
				{
					fprintf( output_file, ",M" );
				}
				else
				{
					fprintf( output_file, ",0" );
				}
			}
			else
			{
				fprintf( output_file, ",0" );
			}
		}
	}

	fprintf( output_file, "\n" );

}

void braun_blanque_clear_aggregate_values( OUTPUT_RECORD *output_record )
{
	output_record->average_height = 0.0;
	output_record->max_height = 0.0;
}

