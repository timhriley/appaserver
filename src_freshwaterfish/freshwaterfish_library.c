/* src_freshwaterfish/freshwaterfish_library.c */
/* ------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"
#include "date_convert.h"
#include "date.h"
#include "folder.h"
#include "hash_table.h"
#include "list_usage.h"
#include "freshwaterfish_library.h"

/* appaserver_link_file */

char *freshwaterfish_library_get_scientific_name(
				char *nps_code,
				char *fiu_fish_old_code,
				char *fiu_non_fish_old_code,
				char *fiu_new_code,
				char *application_name )
{
	static DICTIONARY *nps_code_dictionary = {0};
	static DICTIONARY *fiu_fish_old_code_dictionary = {0};
	static DICTIONARY *fiu_non_fish_old_code_dictionary = {0};
	static DICTIONARY *fiu_new_code_dictionary = {0};
	char *scientific_name = {0};

	if ( !nps_code_dictionary )
	{
		LIST *species_record_list;

		if ( ! ( species_record_list =
			freshwaterfish_get_species_record_list(
				application_name ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get species_record_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

/*
#define SPECIES_SELECT "scientific_name,nps_code,fiu_fish_old_code,fiu_non_fish_old_code,fiu_new_code"
*/
		nps_code_dictionary =
			freshwaterfish_library_get_species_code_dictionary(
				species_record_list,
				1 );

		fiu_fish_old_code_dictionary =
			freshwaterfish_library_get_species_code_dictionary(
				species_record_list,
				2 );

		fiu_non_fish_old_code_dictionary =
			freshwaterfish_library_get_species_code_dictionary(
				species_record_list,
				3 );

		fiu_new_code_dictionary =
			freshwaterfish_library_get_species_code_dictionary(
				species_record_list,
				4 );
	}

	if ( fiu_new_code && *fiu_new_code )
	{
		if ( ( scientific_name =
			dictionary_get_pointer( fiu_new_code_dictionary,
						fiu_new_code ) ) )
		{
			return scientific_name;
		}
		else
		{
			return (char *)0;
		}
	}

	if ( fiu_fish_old_code && *fiu_fish_old_code )
	{
		if ( ( scientific_name =
			dictionary_get_pointer( fiu_fish_old_code_dictionary,
						fiu_fish_old_code ) ) )
		{
			return scientific_name;
		}
		else
		if ( ( scientific_name =
			dictionary_get_pointer( nps_code_dictionary,
						nps_code ) ) )
		{
			return scientific_name;
		}
	}

	if ( fiu_non_fish_old_code && *fiu_non_fish_old_code )
	{
		if ( ( scientific_name =
			dictionary_get_pointer(
					fiu_non_fish_old_code_dictionary,
					fiu_non_fish_old_code ) ) )
		{
			return scientific_name;
		}
		else
		if ( ( scientific_name =
			dictionary_get_pointer( nps_code_dictionary,
						nps_code ) ) )
		{
			return scientific_name;
		}
	}

	return (char *)0;

} /* freshwaterfish_library_get_scientific_name() */

DICTIONARY *freshwaterfish_library_get_species_code_dictionary(
				LIST *species_record_list,
				int code_piece_offset )
{
	char *record;
	char scientific_name[ 128 ];
	char code[ 128 ];
	DICTIONARY *dictionary;

	dictionary = dictionary_small();

	if ( !list_rewind( species_record_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty record list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( species_record_list );

		piece(	scientific_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	code,
			FOLDER_DATA_DELIMITER,
			record,
			code_piece_offset );

		dictionary_set_pointer(
			dictionary,
			strdup( code ),
			strdup( scientific_name ) );

	} while( list_next( species_record_list ) );
	return dictionary;
} /* freshwaterfish_library_get_species_code_dictionary() */

LIST *freshwaterfish_get_species_record_list(
				char *application_name )
{
	LIST *species_record_list;
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=species		",
		 application_name,
		 SPECIES_SELECT );

	species_record_list = pipe2list( sys_string );
	if ( !list_length( species_record_list ) )
		return (LIST *)0;
	else
		return species_record_list;

} /* freshwaterfish_get_species_record_list() */

char *freshwaterfish_get_visit_date_international(
				char *visit_date_compressed )
{
	static char return_date[ 16 ];

	if ( strlen( visit_date_compressed ) != 8 )
	{
		return (char *)0;
	}

	*(return_date + 0) = *visit_date_compressed++;
	*(return_date + 1) = *visit_date_compressed++;
	*(return_date + 2) = *visit_date_compressed++;
	*(return_date + 3) = *visit_date_compressed++;
	*(return_date + 4) = '-';
	*(return_date + 5) = *visit_date_compressed++;
	*(return_date + 6) = *visit_date_compressed++;
	*(return_date + 7) = '-';
	*(return_date + 8) = *visit_date_compressed++;
	*(return_date + 9) = *visit_date_compressed++;
	*(return_date + 10) = '\0';

	return return_date;

} /* freshwaterfish_get_visit_date_international() */

char *freshwaterfish_get_visit_date_compressed(
				char *visit_date_international )
{
	static char return_date[ 16 ];

	if ( strlen( visit_date_international ) != 10 )
	{
		return (char *)0;
	}

	*(return_date + 0) = *visit_date_international++;
	*(return_date + 1) = *visit_date_international++;
	*(return_date + 2) = *visit_date_international++;
	*(return_date + 3) = *visit_date_international++;
	visit_date_international++;
	*(return_date + 4) = *visit_date_international++;
	*(return_date + 5) = *visit_date_international++;
	visit_date_international++;
	*(return_date + 6) = *visit_date_international++;
	*(return_date + 7) = *visit_date_international++;
	*(return_date + 8) = '\0';

	return return_date;

} /* freshwaterfish_get_visit_date_international() */

void freshwaterfish_library_pad_time_four(
				char *throw_time_string )
{
	int str_len;
	char buffer[ 5 ];

	if ( !*throw_time_string )
		strcpy( throw_time_string, "0000" );

	str_len = strlen( throw_time_string );

	if ( str_len > 4 || str_len < 3 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid throw time = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			throw_time_string );
		return;
	}

	if ( str_len == 3 )
	{
		sprintf( buffer, "0%s", throw_time_string );
		strcpy( throw_time_string, buffer );
	}

} /* freshwaterfish_library_pad_time_four() */

boolean freshwaterfish_get_throw_between_dates(
				THROW *throw,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *agency )
{
	char sys_string[ 2048 ];
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		char select[ 1024 ];
		char where_clause[ 1024 ];
		char *folder_list_string;
		char *throw_table_name;
		char *site_visit_table_name;
		char *order_clause = "visit_date,site,plot,region,throw_number";

		throw_table_name = get_table_name( application_name, "throw" );

		site_visit_table_name =
			get_table_name(	application_name,
					"site_visit" );

		sprintf( select,
			 THROW_SELECT_TEMPLATE,
			 site_visit_table_name,
			 site_visit_table_name,
			 site_visit_table_name,
			 site_visit_table_name );

		folder_list_string = "throw,site_visit";

		sprintf(where_clause,
			"%s.site = %s.site and				"
			"%s.plot = %s.plot and				"
			"%s.region = %s.region and			"
			"%s.visit_date = %s.visit_date and		"
		 	"%s.visit_date between '%s' and '%s' and	"
			"%s.agency = '%s'				",
			site_visit_table_name,
			throw_table_name,
			site_visit_table_name,
			throw_table_name,
			site_visit_table_name,
			throw_table_name,
			site_visit_table_name,
			throw_table_name,
			site_visit_table_name,
		 	begin_date_string,
			end_date_string,
			site_visit_table_name,
			agency );

		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=%s		"
			 "			folder=%s		"
			 "			where=\"%s\"		"
			 "			order=\"%s\"		",
			 application_name,
			 select,
			 folder_list_string,
			 where_clause,
			 order_clause );

		input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	freshwaterfish_parse_throw(
			throw,
			buffer );

	throw->surface_vegetation_list =
		freshwaterfish_get_surface_vegetation_list(
			application_name,
			begin_date_string,
			end_date_string,
			throw->site,
			throw->plot,
			throw->region,
			throw->visit_date,
			throw->throw_number );

	throw->submergent_vegetation_list =
		freshwaterfish_get_submergent_vegetation_list(
			application_name,
			begin_date_string,
			end_date_string,
			throw->site,
			throw->plot,
			throw->region,
			throw->visit_date,
			throw->throw_number );

	throw->measurement_list =
		freshwaterfish_get_measurement_list(
			application_name,
			begin_date_string,
			end_date_string,
			throw->site,
			throw->plot,
			throw->region,
			throw->visit_date,
			throw->throw_number );

	return 1;
} /* freshwaterfish_get_throw_between_dates() */

LIST *freshwaterfish_get_measurement_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *site,
			char *plot,
			char *region,
			char *visit_date,
			int throw_number )
{
	static LIST *measurement_record_list = {0};
	LIST *measurement_list = {0};
	char *measurement_record;
	char local_site[ 16 ];
	char local_plot[ 16 ];
	char local_region[ 16 ];
	char local_visit_date[ 16 ];
	char local_throw_number[ 16 ];
	char measurement_number[ 16 ];
	char local_scientific_name[ 128 ];
	char local_length_millimeters[ 16 ];
	char local_sex[ 16 ];
	char local_weight_grams[ 16 ];
	char local_no_measurement_count[ 16 ];
	char local_no_measurement_total_weight[ 16 ];
	char comments[ 1024 ];
	char *length_millimeters;
	char *sex;
	char *weight_grams;
	char *no_measurement_count;
	char *no_measurement_total_weight;
	MEASUREMENT *measurement;

	if ( !measurement_record_list )
	{
		measurement_record_list =
			freshwaterfish_get_measurement_record_list(
				application_name,
				begin_date_string,
				end_date_string );
	}

	if ( !list_rewind( measurement_record_list ) )
		return (LIST *)0;

	do {
		measurement_record =
			list_get_pointer( measurement_record_list );


/*
#define MEASUREMENT_SELECT "site,plot,region,visit_date,throw_number,measurement_number,scientific_name,length_millimeters,sex,weight_grams,no_measurement_count,no_measurement_total_weight,comments"
*/

		piece(	local_site,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			0 );
		if ( strcmp( local_site, site ) != 0 ) continue;

		piece(	local_plot,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			1 );
		if ( strcmp( local_plot, plot ) != 0 ) continue;

		piece(	local_region,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			2 );
		if ( strcmp( local_region, region ) != 0 ) continue;

		piece(	local_visit_date,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			3 );
		if ( strcmp( local_visit_date, visit_date ) != 0 ) continue;

		piece(	local_throw_number,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			4 );
		if ( atoi( local_throw_number ) != throw_number ) continue;

		piece(	measurement_number,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			5 );

		piece(	local_scientific_name,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			6 );

		piece(	local_length_millimeters,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			7 );
		if ( !*local_length_millimeters )
			length_millimeters = (char *)0;
		else
			length_millimeters =
				strdup( local_length_millimeters );

		piece(	local_sex,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			8 );
		if ( !*local_sex )
			sex = (char *)0;
		else
			sex = strdup( local_sex );

		piece(	local_weight_grams,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			9 );
		if ( !*local_weight_grams )
			weight_grams = (char *)0;
		else
			weight_grams = strdup( local_weight_grams );

		piece(	local_no_measurement_count,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			10 );
		if ( !*local_no_measurement_count )
			no_measurement_count = (char *)0;
		else
			no_measurement_count =
				strdup( local_no_measurement_count );

		piece(	local_no_measurement_total_weight,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			11 );
		if ( !*local_no_measurement_total_weight )
			no_measurement_total_weight = (char *)0;
		else
			no_measurement_total_weight =
				strdup( local_no_measurement_total_weight );

		piece(	comments,
			FOLDER_DATA_DELIMITER,
			measurement_record,
			12 );

		measurement =
			freshwaterfish_measurement_new(
				atoi( measurement_number ),
				strdup( local_scientific_name ),
				length_millimeters,
				sex,
				weight_grams,
				no_measurement_count,
				no_measurement_total_weight,
				strdup( comments ) );

		if ( !measurement_list )
			measurement_list = list_new();

		list_append_pointer(	measurement_list,
					measurement );

	} while( list_next( measurement_record_list ) );

	return measurement_list;

} /* freshwaterfish_get_measurement_list() */

void freshwaterfish_parse_throw(
			THROW *throw,
			char *input_buffer )
{
	char buffer[ 1024 ];

/*
#define THROW_SELECT_TEMPLATE	"%s.site,%s.plot,%s.region,%s.visit_date,temperature_celsius,disolved_oxygen_mg_per_liter,conductivity,throw_number,long_grid_number,short_grid_number,throw_time,water_depth_cm,sediment,average_plant_height_cm,emergence_plant_cover_percent,total_plant_cover_percent,periphyton_cover_percent,periphyton_volume_milliliter"
*/
	piece(	throw->site,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	piece(	throw->plot,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		1 );

	piece(	throw->region,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		2 );

	piece(	throw->visit_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		3 );

	piece(	throw->temperature_celsius,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		4 );

	piece(	throw->disolved_oxygen_mg_per_liter,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		5 );

	piece(	throw->conductivity,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		6 );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		7 );
	throw->throw_number = atoi( buffer );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		8 );
	throw->long_grid_number = atoi( buffer );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		9 );
	throw->short_grid_number = atoi( buffer );

	piece(	throw->throw_time,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		10 );

	piece(	throw->water_depth_cm,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		11 );

	piece(	throw->sediment,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		12 );

	piece(	throw->average_plant_height_cm,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		13 );

	piece(	throw->emergence_plant_cover_percent,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		14 );

	piece(	throw->total_plant_cover_percent,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		15 );

	piece(	throw->periphyton_cover_percent,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		16 );

	piece(	throw->periphyton_volume_milliliter,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		17 );

} /* freshwaterfish_parse_throw() */

int freshwaterfish_get_visit_year( char *visit_date )
{
	return atoi( visit_date );
}

int freshwaterfish_get_visit_month( char *visit_date )
{
	char buffer[ 16 ];

	piece( buffer, '-', visit_date, 1 );
	return atoi( buffer );
}

int freshwaterfish_get_visit_day( char *visit_date )
{
	char buffer[ 16 ];

	piece( buffer, '-', visit_date, 2 );
	return atoi( buffer );
}

MEASUREMENT *freshwaterfish_measurement_new(
				int measurement_number,
				char *scientific_name,
				char *length_millimeters,
				char *sex,
				char *weight_grams,
				char *no_measurement_count,
				char *no_measurement_total_weight,
				char *comments )
{
	MEASUREMENT *measurement;

	if ( ! ( measurement =
		calloc( 1, sizeof( MEASUREMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	measurement->measurement_number = measurement_number;
	measurement->scientific_name = scientific_name;

	if ( length_millimeters && *length_millimeters )
	{
		measurement->length_millimeters =
			length_millimeters;
	}
	else
	{
		measurement->length_millimeters = ".";
	}

	if ( sex && *sex )
	{
		measurement->sex =
			sex;
	}
	else
	{
		measurement->sex = ".";
	}

	if ( weight_grams && *weight_grams )
	{
		measurement->weight_grams =
			weight_grams;
	}
	else
	{
		measurement->weight_grams = ".";
	}

	if ( no_measurement_count && *no_measurement_count )
	{
		measurement->no_measurement_count =
			no_measurement_count;
	}
	else
	{
		measurement->no_measurement_count = ".";
	}

	if ( no_measurement_total_weight && *no_measurement_total_weight )
	{
		measurement->no_measurement_total_weight =
			no_measurement_total_weight;
	}
	else
	{
		measurement->no_measurement_total_weight = ".";
	}

	if ( comments && *comments )
	{
		measurement->comments =
			comments;
	}
	else
	{
		measurement->comments = ".";
	}

	return measurement;
} /* freshwaterfish_measurement_new() */

SURFACE_VEGETATION *freshwaterfish_surface_vegetation_new(
				char *scientific_name,
				char *density_alive_count,
				char *density_dead_count )
{
	SURFACE_VEGETATION *surface_vegetation;

	if ( ! ( surface_vegetation =
		calloc( 1, sizeof( SURFACE_VEGETATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	surface_vegetation->scientific_name = scientific_name;

	if ( density_alive_count && *density_alive_count )
	{
		surface_vegetation->density_alive_count =
			density_alive_count;
	}
	else
	{
		surface_vegetation->density_alive_count = ".";
	}

	if ( density_dead_count && *density_dead_count )
	{
		surface_vegetation->density_dead_count =
			density_dead_count;
	}
	else
	{
		surface_vegetation->density_dead_count = ".";
	}

	return surface_vegetation;
} /* freshwaterfish_surface_vegetation_new() */

SUBMERGENT_VEGETATION *freshwaterfish_submergent_vegetation_new(
				char *scientific_name,
				char *density_percent )
{
	SUBMERGENT_VEGETATION *submergent_vegetation;

	if ( ! ( submergent_vegetation =
		calloc( 1, sizeof( SUBMERGENT_VEGETATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	submergent_vegetation->scientific_name = scientific_name;

	if ( density_percent && *density_percent )
	{
		submergent_vegetation->density_percent =
			density_percent;
	}
	else
	{
		submergent_vegetation->density_percent = ".";
	}

	return submergent_vegetation;
} /* freshwaterfish_submergent_vegetation_new() */

LIST *freshwaterfish_get_measurement_record_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string )
{
	LIST *measurement_record_list;
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];

	sprintf( where_clause,
		 "visit_date between '%s' and '%s'",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=measurement		"
		 "			where=\"%s\"			"
		 "			order=measurement_number	",
		 application_name,
		 MEASUREMENT_SELECT,
		 where_clause );

	measurement_record_list = pipe2list( sys_string );
	if ( !list_length( measurement_record_list ) )
		return (LIST *)0;
	else
		return measurement_record_list;

} /* freshwaterfish_get_measurement_record_list() */

LIST *freshwaterfish_get_surface_vegetation_record_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string )
{
	LIST *surface_vegetation_record_list;
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];

	sprintf( where_clause,
		 "visit_date between '%s' and '%s'",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=surface_vegetation	"
		 "			where=\"%s\"			",
		 application_name,
		 SURFACE_VEGETATION_SELECT,
		 where_clause );

	surface_vegetation_record_list = pipe2list( sys_string );
	if ( !list_length( surface_vegetation_record_list ) )
		return (LIST *)0;
	else
		return surface_vegetation_record_list;

} /* freshwaterfish_get_surface_vegetation_record_list() */

LIST *freshwaterfish_get_submergent_vegetation_record_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string )
{
	LIST *submergent_vegetation_record_list;
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];

	sprintf( where_clause,
		 "visit_date between '%s' and '%s'",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=submergent_vegetation	"
		 "			where=\"%s\"			",
		 application_name,
		 SUBMERGENT_VEGETATION_SELECT,
		 where_clause );

	submergent_vegetation_record_list = pipe2list( sys_string );
	if ( !list_length( submergent_vegetation_record_list ) )
		return (LIST *)0;
	else
		return submergent_vegetation_record_list;

} /* freshwaterfish_get_submergent_vegetation_record_list() */

LIST *freshwaterfish_get_surface_vegetation_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *site,
			char *plot,
			char *region,
			char *visit_date,
			int throw_number )
{
	static LIST *surface_vegetation_record_list = {0};
	LIST *surface_vegetation_list = {0};
	char *surface_vegetation_record;
	char local_site[ 16 ];
	char local_plot[ 16 ];
	char local_region[ 128 ];
	char local_visit_date[ 16 ];
	char local_throw_number[ 16 ];
	char local_scientific_name[ 128 ];
	char local_density_alive_count[ 16 ];
	char local_density_dead_count[ 16 ];
	char *density_alive_count;
	char *density_dead_count;
	SURFACE_VEGETATION *surface_vegetation;

	if ( !surface_vegetation_record_list )
	{
		surface_vegetation_record_list =
			freshwaterfish_get_surface_vegetation_record_list(
				application_name,
				begin_date_string,
				end_date_string );
	}

	if ( !list_rewind( surface_vegetation_record_list ) )
		return (LIST *)0;

	do {
		surface_vegetation_record =
			list_get_pointer( surface_vegetation_record_list );


/*
#define SURFACE_VEGETATION_SELECT "site,plot,region,visit_date,throw_number,scientific_name,density_alive_count,density_dead_count"
*/

		piece(	local_site,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			0 );
		if ( strcmp( local_site, site ) != 0 ) continue;

		piece(	local_plot,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			1 );
		if ( strcmp( local_plot, plot ) != 0 ) continue;

		piece(	local_region,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			2 );
		if ( strcmp( local_region, region ) != 0 ) continue;

		piece(	local_visit_date,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			3 );
		if ( strcmp( local_visit_date, visit_date ) != 0 ) continue;

		piece(	local_throw_number,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			4 );
		if ( atoi( local_throw_number ) != throw_number ) continue;

		piece(	local_scientific_name,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			5 );

		piece(	local_density_alive_count,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			6 );
		if ( !*local_density_alive_count )
			density_alive_count = (char *)0;
		else
			density_alive_count =
				strdup( local_density_alive_count );

		piece(	local_density_dead_count,
			FOLDER_DATA_DELIMITER,
			surface_vegetation_record,
			7 );
		if ( !*local_density_dead_count )
			density_dead_count = (char *)0;
		else
			density_dead_count =
				strdup( local_density_dead_count );

		surface_vegetation =
			freshwaterfish_surface_vegetation_new(
				strdup( local_scientific_name ),
				density_alive_count,
				density_dead_count );

		if ( !surface_vegetation_list )
			surface_vegetation_list = list_new();

		list_append_pointer(	surface_vegetation_list,
					surface_vegetation );

	} while( list_next( surface_vegetation_record_list ) );

	return surface_vegetation_list;

} /* freshwaterfish_get_surface_vegetation_list() */

LIST *freshwaterfish_get_submergent_vegetation_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *site,
			char *plot,
			char *region,
			char *visit_date,
			int throw_number )
{
	static LIST *submergent_vegetation_record_list = {0};
	LIST *submergent_vegetation_list = {0};
	char *submergent_vegetation_record;
	char local_site[ 16 ];
	char local_plot[ 16 ];
	char local_region[ 128 ];
	char local_visit_date[ 16 ];
	char local_throw_number[ 16 ];
	char local_scientific_name[ 128 ];
	char local_density_percent[ 16 ];
	char *density_percent;
	SUBMERGENT_VEGETATION *submergent_vegetation;

	if ( !submergent_vegetation_record_list )
	{
		submergent_vegetation_record_list =
			freshwaterfish_get_submergent_vegetation_record_list(
				application_name,
				begin_date_string,
				end_date_string );
	}

	if ( !list_rewind( submergent_vegetation_record_list ) )
		return (LIST *)0;

	do {
		submergent_vegetation_record =
			list_get_pointer( submergent_vegetation_record_list );


/*
#define SUBMERGENT_VEGETATION_SELECT "site,plot,region,visit_date,throw_number,scientific_name,density_percent"
*/

		piece(	local_site,
			FOLDER_DATA_DELIMITER,
			submergent_vegetation_record,
			0 );
		if ( strcmp( local_site, site ) != 0 ) continue;

		piece(	local_plot,
			FOLDER_DATA_DELIMITER,
			submergent_vegetation_record,
			1 );
		if ( strcmp( local_plot, plot ) != 0 ) continue;

		piece(	local_region,
			FOLDER_DATA_DELIMITER,
			submergent_vegetation_record,
			2 );
		if ( strcmp( local_region, region ) != 0 ) continue;

		piece(	local_visit_date,
			FOLDER_DATA_DELIMITER,
			submergent_vegetation_record,
			3 );
		if ( strcmp( local_visit_date, visit_date ) != 0 ) continue;

		piece(	local_throw_number,
			FOLDER_DATA_DELIMITER,
			submergent_vegetation_record,
			4 );
		if ( atoi( local_throw_number ) != throw_number ) continue;

		piece(	local_scientific_name,
			FOLDER_DATA_DELIMITER,
			submergent_vegetation_record,
			5 );

		piece(	local_density_percent,
			FOLDER_DATA_DELIMITER,
			submergent_vegetation_record,
			6 );
		if ( !*local_density_percent )
			density_percent = (char *)0;
		else
			density_percent =
				strdup( local_density_percent );

		submergent_vegetation =
			freshwaterfish_submergent_vegetation_new(
				strdup( local_scientific_name ),
				density_percent );

		if ( !submergent_vegetation_list )
			submergent_vegetation_list = list_new();

		list_append_pointer(	submergent_vegetation_list,
					submergent_vegetation );

	} while( list_next( submergent_vegetation_record_list ) );

	return submergent_vegetation_list;

} /* freshwaterfish_get_submergent_vegetation_list() */

char *freshwaterfish_get_nps_code(
				char *application_name,
				char *scientific_name )
{
	static DICTIONARY *nps_code_dictionary = {0};
	char *code_pointer;
	char local_code[ 128 ];
	char local_scientific_name[ 128 ];

	if ( !nps_code_dictionary )
	{
		LIST *species_record_list;
		char *record;

		if ( ! ( species_record_list =
			freshwaterfish_get_species_record_list(
				application_name ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get species_record_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

/*
#define SPECIES_SELECT "scientific_name,nps_code,fiu_fish_old_code,fiu_non_fish_old_code,fiu_new_code"
*/
		nps_code_dictionary = dictionary_small();

		if ( !list_rewind( species_record_list ) )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty record list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			record = list_get_pointer( species_record_list );

			piece(	local_scientific_name,
				FOLDER_DATA_DELIMITER,
				record,
				0 );

			piece(	local_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			dictionary_set_pointer(
				nps_code_dictionary,
				strdup( local_scientific_name ),
				strdup( local_code ) );

		} while( list_next( species_record_list ) );
	}

	if ( ( code_pointer =
		dictionary_get_pointer( nps_code_dictionary,
					scientific_name ) ) )
	{
		if ( !*code_pointer )
			return ".";
		else
			return code_pointer;
	}
	else
	{
		return ".";
	}

} /* freshwaterfish_get_nps_code() */

char *freshwaterfish_get_fiu_new_code(
				char *application_name,
				char *scientific_name )
{
	static DICTIONARY *fiu_new_code_dictionary = {0};
	char *code_pointer;
	static char fiu_new_code[ 128 ];
	char local_code[ 128 ];
	char local_scientific_name[ 128 ];

	if ( !fiu_new_code_dictionary )
	{
		LIST *species_record_list;
		char *record;

		if ( ! ( species_record_list =
			freshwaterfish_get_species_record_list(
				application_name ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get species_record_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

/*
#define SPECIES_SELECT "scientific_name,nps_code,fiu_fish_old_code,fiu_non_fish_old_code,fiu_new_code"
*/
		fiu_new_code_dictionary = dictionary_small();

		if ( !list_rewind( species_record_list ) )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty record list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			record = list_get_pointer( species_record_list );

			piece(	local_scientific_name,
				FOLDER_DATA_DELIMITER,
				record,
				0 );

			piece(	local_code,
				FOLDER_DATA_DELIMITER,
				record,
				4 );

			dictionary_set_pointer(
				fiu_new_code_dictionary,
				strdup( local_scientific_name ),
				strdup( local_code ) );

		} while( list_next( species_record_list ) );
	}

	if ( ( code_pointer =
		dictionary_get_pointer( fiu_new_code_dictionary,
					scientific_name ) ) )
	{
		if ( !*code_pointer )
		{
			return ".";
		}
		else
		{
			strcpy( fiu_new_code, code_pointer );
			up_string( fiu_new_code );
			return fiu_new_code;
		}
	}
	else
	{
		return ".";
	}

} /* freshwaterfish_get_fiu_new_code() */

char *freshwaterfish_get_fiu_non_fish_old_code(
				char *application_name,
				char *scientific_name )
{
	static DICTIONARY *fiu_non_fish_old_code_dictionary = {0};
	char *code_pointer;
	char local_code[ 128 ];
	char local_scientific_name[ 128 ];

	if ( !fiu_non_fish_old_code_dictionary )
	{
		LIST *species_record_list;
		char *record;

		if ( ! ( species_record_list =
			freshwaterfish_get_species_record_list(
				application_name ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get species_record_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

/*
#define SPECIES_SELECT "scientific_name,nps_code,fiu_fish_old_code,fiu_non_fish_old_code,fiu_new_code"
*/
		fiu_non_fish_old_code_dictionary = dictionary_small();

		if ( !list_rewind( species_record_list ) )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty record list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			record = list_get_pointer( species_record_list );

			piece(	local_scientific_name,
				FOLDER_DATA_DELIMITER,
				record,
				0 );

			piece(	local_code,
				FOLDER_DATA_DELIMITER,
				record,
				3 );

			dictionary_set_pointer(
				fiu_non_fish_old_code_dictionary,
				strdup( local_scientific_name ),
				strdup( local_code ) );

		} while( list_next( species_record_list ) );
	}

	if ( ( code_pointer =
		dictionary_get_pointer( fiu_non_fish_old_code_dictionary,
					scientific_name ) ) )
	{
		if ( !*code_pointer )
			return ".";
		else
			return code_pointer;
	}
	else
	{
		return ".";
	}

} /* freshwaterfish_get_fiu_non_fish_old_code() */

char *freshwaterfish_get_fiu_fish_old_code(
				char *application_name,
				char *scientific_name )
{
	static DICTIONARY *fiu_fish_old_code_dictionary = {0};
	char *code_pointer;
	char local_code[ 128 ];
	char local_scientific_name[ 128 ];

	if ( !fiu_fish_old_code_dictionary )
	{
		LIST *species_record_list;
		char *record;

		if ( ! ( species_record_list =
			freshwaterfish_get_species_record_list(
				application_name ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get species_record_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

/*
#define SPECIES_SELECT "scientific_name,nps_code,fiu_fish_old_code,fiu_non_fish_old_code,fiu_new_code"
*/
		fiu_fish_old_code_dictionary = dictionary_small();

		if ( !list_rewind( species_record_list ) )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty record list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			record = list_get_pointer( species_record_list );

			piece(	local_scientific_name,
				FOLDER_DATA_DELIMITER,
				record,
				0 );

			piece(	local_code,
				FOLDER_DATA_DELIMITER,
				record,
				2 );

			dictionary_set_pointer(
				fiu_fish_old_code_dictionary,
				strdup( local_scientific_name ),
				strdup( local_code ) );

		} while( list_next( species_record_list ) );
	}

	if ( ( code_pointer =
		dictionary_get_pointer( fiu_fish_old_code_dictionary,
					scientific_name ) ) )
	{
		if ( !*code_pointer )
			return ".";
		else
			return code_pointer;
	}
	else
	{
		return ".";
	}

} /* freshwaterfish_get_fiu_fish_old_code() */

char *freshwaterfish_get_sex(
				char *application_name,
				char *sex_alpha_code )
{
	static LIST *sex_record_list = {0};
	char *record;
	char search_string[ 128 ];
	int str_len;
	static char sex[ 128 ];

	if ( !sex_record_list )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
		 	"get_folder_data	application=%s		"
		 	"			select=%s		"
		 	"			folder=sex		",
		 	application_name,
		 	"sex_alpha_code,sex" );

		sex_record_list = pipe2list( sys_string );
	}

	if ( !*sex_alpha_code || *sex_alpha_code == '.' ) return UNKNOWN_SEX;

	sprintf(	search_string,
			"%s%c", 
			sex_alpha_code,
			FOLDER_DATA_DELIMITER );
	str_len = strlen( search_string );

	if ( !list_rewind( sex_record_list ) )
		return (char *)0;

	do {
		record = list_get_pointer( sex_record_list );

		if ( strncmp(	record,
				search_string,
				str_len ) == 0 )
		{
			piece(	sex,
				FOLDER_DATA_DELIMITER,
				record,
				1 );
			return sex;
		}

	} while( list_next( sex_record_list ) );

	return (char *)0;

} /* freshwaterfish_get_sex() */

char *freshwaterfish_get_sex_fiu_code(	char *application_name,
					char *sex )
{
	static LIST *sex_record_list = {0};
	char *record;
	char search_string[ 128 ];
	int str_len;
	static char sex_fiu_code[ 8 ];

	if ( !sex_record_list )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
		 	"get_folder_data	application=%s		"
		 	"			select=%s		"
		 	"			folder=sex		",
		 	application_name,
		 	"sex,sex_fiu_code" );

		sex_record_list = pipe2list( sys_string );
	}

	sprintf(	search_string,
			"%s%c", 
			sex,
			FOLDER_DATA_DELIMITER );
	str_len = strlen( search_string );

	if ( !list_rewind( sex_record_list ) )
		return (char *)0;

	do {
		record = list_get_pointer( sex_record_list );

		if ( strncmp(	record,
				search_string,
				str_len ) == 0 )
		{
			piece(	sex_fiu_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );
			return sex_fiu_code;
		}

	} while( list_next( sex_record_list ) );

	return (char *)0;

} /* freshwaterfish_get_sex_fiu_code() */

char *freshwaterfish_get_sex_alpha_code(char *application_name,
					char *sex )
{
	static LIST *sex_record_list = {0};
	char *record;
	char search_string[ 128 ];
	int str_len;
	static char sex_alpha_code[ 8 ];

	if ( !sex_record_list )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
		 	"get_folder_data	application=%s		"
		 	"			select=%s		"
		 	"			folder=sex		",
		 	application_name,
		 	"sex,sex_alpha_code" );

		sex_record_list = pipe2list( sys_string );
	}

	sprintf(	search_string,
			"%s%c", 
			sex,
			FOLDER_DATA_DELIMITER );
	str_len = strlen( search_string );

	if ( !list_rewind( sex_record_list ) )
		return (char *)0;

	do {
		record = list_get_pointer( sex_record_list );

		if ( strncmp(	record,
				search_string,
				str_len ) == 0 )
		{
			piece(	sex_alpha_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );
			return sex_alpha_code;
		}

	} while( list_next( sex_record_list ) );

	return (char *)0;

} /* freshwaterfish_get_sex_alpha_code() */

char *freshwaterfish_with_fiu_code_get_sex(
				char *application_name,
				char *fiu_sex_code )
{
	static LIST *sex_record_list = {0};
	int str_len;
	char search_string[ 16 ];
	static char sex[ 128 ];
	char *record;

	if ( !sex_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "sex_fiu_code,sex";

		sprintf(sys_string,
			"get_folder_data	application=%s	"
			"			select=%s	"
			"			folder=sex	",
			application_name,
			select );

		sex_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( sex_record_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty sex_record_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( search_string, "%s^", fiu_sex_code );
	str_len = strlen( search_string );
	*sex = '\0';

	do {
		record = list_get_pointer( sex_record_list );

		if ( strncmp( search_string, record, str_len ) == 0 )
		{
			piece( sex, '^', record, 1 );
			return sex;
		}
	} while( list_next( sex_record_list ) );
	return (char *)0;

} /* freshwaterfish_with_fiu_code_get_sex() */

char *freshwaterfish_get_region(	char *application_name,
					char *region_code )
{
	static LIST *region_record_list = {0};
	int str_len;
	char search_string[ 16 ];
	static char region[ 128 ];
	char *record;

	if ( !region_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "region_code,region";

		sprintf(sys_string,
			"get_folder_data	application=%s	"
			"			select=%s	"
			"			folder=region	",
			application_name,
			select );

		region_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( region_record_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty region_record_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( search_string, "%s^", region_code );
	str_len = strlen( search_string );
	*region = '\0';

	do {
		record = list_get_pointer( region_record_list );

		if ( strncmp( search_string, record, str_len ) == 0 )
		{
			piece( region, '^', record, 1 );
			return region;
		}
	} while( list_next( region_record_list ) );
	return (char *)0;

} /* freshwaterfish_get_region() */

char *freshwaterfish_get_nps_region( char *site )
{
	if ( strcmp( site, "6" ) == 0 )
		return NPS_REGION_SHARK_RIVER_SLOUGH;
	else
	if ( strcmp( site, "50" ) == 0 )
		return NPS_REGION_SHARK_RIVER_SLOUGH;
	else
	if ( strcmp( site, "51" ) == 0 )
		return NPS_REGION_SHARK_RIVER_SLOUGH;
	else
	if ( strcmp( site, "23" ) == 0 )
		return NPS_REGION_NORTHEAST_SHARK_SLOUGH;
	else
		return (char *)0;

} /* freshwaterfish_get_nps_region() */

boolean freshwaterfish_exists_region(
					char *application_name,
					char *region )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf( where, "region = '%s'", region );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select='count(*)'	 "
		 "			folder=region		 "
		 "			where=\"%s\"		|"
		 "cat						 ",
		 application_name,
		 where );

	return atoi( pipe2string( sys_string ) );

} /* freshwaterfish_exists_region() */

boolean freshwaterfish_exists_agency(
					char *application_name,
					char *agency )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf( where, "agency = '%s'", agency );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select='count(*)'	 "
		 "			folder=agency		 "
		 "			where=\"%s\"		|"
		 "cat						 ",
		 application_name,
		 where );

	return atoi( pipe2string( sys_string ) );

} /* freshwaterfish_exists_agency() */

boolean freshwaterfish_exists_sex(
					char *application_name,
					char *sex )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf( where, "sex = '%s'", sex );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select='count(*)'	 "
		 "			folder=sex		 "
		 "			where=\"%s\"		|"
		 "cat						 ",
		 application_name,
		 where );

	return atoi( pipe2string( sys_string ) );

} /* freshwaterfish_exists_sex() */

SPECIES_GROUP_WEIGHT *freshwaterfish_species_group_weight_new(
					char *application_name,
					LIST *species_group_name_list )
{
	SPECIES_GROUP_WEIGHT *species_group_weight;
	SPECIES_GROUP *species_group;
	char *species_group_name;

	if ( ! ( species_group_weight =
		calloc( 1, sizeof( SPECIES_GROUP_WEIGHT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	species_group_weight->species_group_list = list_new();

	if ( list_rewind( species_group_name_list ) )
	{
		do {
			species_group_name =
				list_get_pointer( species_group_name_list );

			species_group =
				freshwaterfish_species_group_new(
					application_name,
					species_group_name,
					0 /* not with_membership_dictionary */);

			list_append_pointer(	species_group_weight->
							species_group_list,
						species_group );
		} while( list_next( species_group_name_list ) );
	}

	return species_group_weight;
} /* freshwaterfish_species_group_weight_new() */

MEASUREMENT_COUNT *freshwaterfish_measurement_count_new(
				char *application_name,
				LIST *species_group_name_list )
{
	MEASUREMENT_COUNT *measurement_count;

	if ( ! ( measurement_count =
		calloc( 1, sizeof( MEASUREMENT_COUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	measurement_count->species_comprised_list =
		freshwaterfish_get_species_comprised_list(
				application_name );

	measurement_count->species_list =
		freshwaterfish_get_species_list(
				application_name,
				species_group_name_list,
				measurement_count->species_comprised_list );

	return measurement_count;
} /* freshwaterfish_measurement_count_new() */

THROW_WEIGHT_COUNT *freshwaterfish_throw_weight_count_new(
				char *site,
				char *plot,
				char *visit_date,
				int throw_number )
{
	THROW_WEIGHT_COUNT *throw;

	if ( ! ( throw =
		calloc( 1, sizeof( THROW_WEIGHT_COUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	throw->site = site;
	throw->plot = plot;
	throw->visit_date = visit_date;
	throw->throw_number = throw_number;
	throw->species_hash_table = hash_table_new( hash_table_small );
	throw->species_group_hash_table = hash_table_new( hash_table_small );

	return throw;
} /* freshwaterfish_throw_weight_count_new() */

LIST *freshwaterfish_get_species_list(
					char *application_name,
					LIST *species_group_name_list,
					LIST *species_comprised_list )
{
	SPECIES *species;
	LIST *species_list;
	char sys_string[ 1024 ];
	char *where_in_clause;
	char *species_table_name;
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	char select[ 128 ];
	char *folder;
	char *order;
	FILE *input_pipe;
	LIST *scientific_name_comprised_list;

	scientific_name_comprised_list =
		freshwaterfish_get_scientific_name_comprised_list(
				species_comprised_list );

	species_list = list_new();

	species_table_name = get_table_name( application_name, "species" );

	sprintf( select,
		 "%s.scientific_name,fiu_non_fish_old_code,fiu_new_code",
		 species_table_name );

	folder = "species";
	where_in_clause =
		freshwaterfish_get_species_category_where_in_clause(
					application_name,
					species_group_name_list );
	order = "fiu_new_code";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	"
		 "			order=%s	",
		 application_name,
		 select,
		 folder,
		 where_in_clause,
		 order );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		/* Get scientific_name */
		/* ------------------- */
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		if ( list_exists_string(
			piece_buffer,
			scientific_name_comprised_list ) )
		{
			continue;
		}

		species = freshwaterfish_species_new(
				strdup( piece_buffer ) );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		species->fiu_non_fish_old_code = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
		species->fiu_new_code = strdup( piece_buffer );

		list_append_pointer( species_list, species );
	}

	pclose( input_pipe );
	return species_list;
} /* freshwaterfish_get_species_list() */

char *freshwaterfish_get_species_category_where_in_clause(
				char *application_name,
				LIST *species_category_name_list )
{
	static char where[ 256 ];
	char *table_name;
	char *in_clause;

	table_name = get_table_name(	application_name,
					"species" );

	in_clause = list_usage_get_in_clause( species_category_name_list );
	sprintf(	where,
			"%s.species_category in %s",
			table_name,
			in_clause );
	return where;
} /* freshwaterfish_get_species_category_where_in_clause() */

SPECIES *freshwaterfish_species_new(
				char *scientific_name )
{
	SPECIES *species;

	if ( ! ( species =
		calloc( 1, sizeof( SPECIES ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	species->scientific_name = scientific_name;

	return species;
} /* freshwaterfish_species_new() */

boolean freshwaterfish_in_species_membership(
					char *application_name,
					LIST *species_group_name_list,
					char *scientific_name )
{
	char *species_group_name;
	boolean return_value;

	if ( !list_rewind( species_group_name_list ) ) return 0;

	do {
		species_group_name =
			list_get_pointer(
				species_group_name_list );

		return_value =
			freshwaterfish_species_in_species_membership(
				application_name,
				species_group_name,
				scientific_name );
		if ( return_value ) return 1;
	} while( list_next( species_group_name_list ) );
	return 0;
} /* freshwaterfish_in_species_membership() */

boolean freshwaterfish_species_in_species_membership(
				char *application_name,
				char *species_group_name,
				char *scientific_name )
{
	static LIST *species_group_list = {0};
	SPECIES_GROUP *species_group;

	if ( !species_group_list )
	{
		species_group_list =
			freshwaterfish_get_species_group_list(
				application_name );
	}

	if ( list_rewind( species_group_list ) )
	{
		do {
			species_group = list_get_pointer( species_group_list );

			if ( strcmp(	species_group->species_group_name,
					species_group_name ) == 0 )
			{

				return dictionary_exists_key(
					species_group->membership_dictionary,
					scientific_name );
			}
		} while( list_next( species_group_list ) );
	}

	fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot find species_group_name = (%s)\n",
		__FILE__,
		__FUNCTION__,
		__LINE__,
		species_group_name );
	exit( 1 );

} /* freshwaterfish_species_in_species_membership() */

LIST *freshwaterfish_get_species_group_list(
				char *application_name )
{
	LIST *species_group_list = list_new();
	LIST *species_group_name_list;
	char *species_group_name;
	SPECIES_GROUP *species_group;

	species_group_name_list =
		freshwaterfish_get_species_group_name_list(
			application_name );

	if ( list_rewind( species_group_name_list ) )
	{
		do {
			species_group_name =
				list_get_pointer(
					species_group_name_list );

			species_group =
				freshwaterfish_species_group_new(
					application_name,
					species_group_name,
					1 /* with_membership_dictionary */ );

			list_append_pointer(
				species_group_list,
				species_group );
		} while( list_next( species_group_name_list ) );
	}
	return species_group_list;
} /* freshwaterfish_get_species_group_list() */

SPECIES_GROUP *freshwaterfish_species_group_new(
				char *application_name,
				char *species_group_name,
				boolean with_membership_dictionary )
{
	SPECIES_GROUP *species_group;

	if ( ! ( species_group =
		calloc( 1, sizeof( SPECIES_GROUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	species_group->species_group_name = species_group_name;

	if ( with_membership_dictionary )
	{
		species_group->membership_dictionary =
			freshwaterfish_get_membership_dictionary(
				application_name,
				species_group_name );
	}

	return species_group;
} /* freshwaterfish_species_group_new() */

DICTIONARY *freshwaterfish_get_membership_dictionary(
			char *application_name,
			char *species_category_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf( where, "species_category = '%s'", species_category_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=scientific_name		"
		"			folder=species			"
		"			where=\"%s\"			",
		application_name,
		where );

	return pipe2dictionary( sys_string, 0 /* delimiter */ );
} /* freshwaterfish_get_membership_dictionary() */

LIST *freshwaterfish_get_species_group_name_list(
			char *application_name )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=species_category		"
		"			folder=species_category		",
		application_name );

	return pipe2list( sys_string );
} /* freshwaterfish_get_species_group_name_list() */

LIST *freshwaterfish_get_species_throw_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *select_shell_script,
			LIST *species_comprised_list )
{
	char sys_string[ 1024 ];
	LIST *throw_list;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char visit_date[ 16 ];
	char site[ 16 ];
	char plot[ 16 ];
	char throw_number[ 16 ];
	char old_visit_date[ 16 ] = {0};
	char old_site[ 16 ];
	char old_plot[ 16 ];
	char old_throw_number[ 16 ];
	char scientific_name[ 128 ];
	char aggregation[ 16 ];
	THROW_WEIGHT_COUNT *throw = {0};
	SPECIES *species;
	SPECIES_COMPRISED *species_comprised;

	sprintf(sys_string,
	 	"%s %s %s %s",
		select_shell_script,
	 	application_name,
	 	begin_date_string,
	 	end_date_string );

	throw_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{

/* ----------------------------------------------------------------------- */
/* select								   */
/* visit_date,site,plot,throw_number,scientific_name,aggregation(*)	   */
/* ----------------------------------------------------------------------- */

		piece(	visit_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	site,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	plot,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	throw_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	scientific_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		if ( ( species_comprised =
			freshwaterfish_get_species_comprised(
				species_comprised_list,
				scientific_name ) ) )
		{
			timlib_strcpy(	scientific_name,
					species_comprised->scientific_name,
					128 );
		}

		piece(	aggregation,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		/* If first time */
		/* ------------- */
		if ( !*old_visit_date )
		{
			throw =
				freshwaterfish_throw_weight_count_new(
				strdup( site ),
				strdup( plot ),
				strdup( visit_date ),
				atoi( throw_number ) );

			list_append_pointer( throw_list, throw );

			strcpy( old_visit_date, visit_date );
			strcpy( old_site, site );
			strcpy( old_plot, plot );
			strcpy( old_throw_number, throw_number );
		}
		else
		/* -------------------- */
		/* If a change in throw */
		/* -------------------- */
		if ( strcmp( old_visit_date, visit_date ) != 0
		||   strcmp( old_site, site ) != 0
		||   strcmp( old_plot, plot ) != 0
		||   strcmp( old_throw_number, throw_number ) != 0 )
		{
			throw =
				freshwaterfish_throw_weight_count_new(
				strdup( site ),
				strdup( plot ),
				strdup( visit_date ),
				atoi( throw_number ) );

			list_append_pointer( throw_list, throw );

			strcpy( old_visit_date, visit_date );
			strcpy( old_site, site );
			strcpy( old_plot, plot );
			strcpy( old_throw_number, throw_number );
		}

		if ( ( species = hash_table_get_pointer(
					throw->species_hash_table,
					scientific_name ) ) )
		{
			species->aggregation += atof( aggregation );
		}
		else
		{
			species = freshwaterfish_species_new(
					strdup( scientific_name ) );

			species->aggregation = atof( aggregation );

			hash_table_set_pointer(
				throw->species_hash_table,
				species->scientific_name,
				species );
		}
	}
	pclose( input_pipe );
	return throw_list;
} /* freshwaterfish_get_species_throw_list() */

LIST *freshwaterfish_get_species_group_throw_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string )
{
	char sys_string[ 1024 ];
	LIST *throw_list;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char visit_date[ 16 ];
	char site[ 16 ];
	char plot[ 16 ];
	char throw_number[ 16 ];
	char old_visit_date[ 16 ] = {0};
	char old_site[ 16 ];
	char old_plot[ 16 ];
	char old_throw_number[ 16 ];
	char species_group_name[ 128 ];
	char weight_sum[ 16 ];
	char count[ 16 ];
	THROW_WEIGHT_COUNT *throw = {0};
	SPECIES_GROUP *species_group;

	sprintf(sys_string,
	 	"select_wet_weight.sh %s %s %s",
	 	application_name,
	 	begin_date_string,
	 	end_date_string );

	throw_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
/* ----------------------------------------------------------------------- */
/* select								   */
/* visit_date,site,plot,throw_number,species_group,sum(weight),count(*)	   */
/* ----------------------------------------------------------------------- */

		piece(	visit_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	site,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	plot,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	throw_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	species_group_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	weight_sum,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		piece(	count,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );

		/* If first time */
		/* ------------- */
		if ( !*old_visit_date )
		{
			throw =
				freshwaterfish_throw_weight_count_new(
				strdup( site ),
				strdup( plot ),
				strdup( visit_date ),
				atoi( throw_number ) );

			list_append_pointer( throw_list, throw );

			strcpy( old_visit_date, visit_date );
			strcpy( old_site, site );
			strcpy( old_plot, plot );
			strcpy( old_throw_number, throw_number );
		}
		else
		/* -------------------- */
		/* If a change in throw */
		/* -------------------- */
		if ( strcmp( old_visit_date, visit_date ) != 0
		||   strcmp( old_site, site ) != 0
		||   strcmp( old_plot, plot ) != 0
		||   strcmp( old_throw_number, throw_number ) != 0 )
		{
			throw =
				freshwaterfish_throw_weight_count_new(
				strdup( site ),
				strdup( plot ),
				strdup( visit_date ),
				atoi( throw_number ) );

			list_append_pointer( throw_list, throw );

			strcpy( old_visit_date, visit_date );
			strcpy( old_site, site );
			strcpy( old_plot, plot );
			strcpy( old_throw_number, throw_number );
		}

		species_group =
			freshwaterfish_species_group_new(
				application_name,
				strdup( species_group_name ),
				0 /* not with_membership_dictionary */ );

		species_group->weight_sum = atof( weight_sum );
		species_group->count = atoi( count );

		hash_table_set_pointer(
			throw->species_group_hash_table,
			species_group->species_group_name,
			species_group );
	}
	pclose( input_pipe );
	return throw_list;
} /* freshwaterfish_get_species_group_throw_list() */

LIST *freshwaterfish_get_scientific_name_comprised_list(
				LIST *species_comprised_list )
{
	SPECIES_COMPRISED *species_comprised;
	LIST *scientific_name_comprised_list;

	if ( !list_rewind( species_comprised_list ) )
		return (LIST *)0;

	scientific_name_comprised_list = list_new();

	do {
		species_comprised = list_get_pointer( species_comprised_list );

		list_append_pointer(
			scientific_name_comprised_list,
			species_comprised->scientific_name_comprised );
	} while( list_next( species_comprised_list ) );
		
	return scientific_name_comprised_list;

} /* freshwaterfish_get_scientific_name_comprised_list() */

SPECIES_COMPRISED *freshwaterfish_species_comprised_new(
				char *scientific_name,
				char *scientific_name_comprised )
{
	SPECIES_COMPRISED *species_comprised;

	if ( ! ( species_comprised =
		calloc( 1, sizeof( SPECIES_COMPRISED ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	species_comprised->scientific_name = scientific_name;
	species_comprised->scientific_name_comprised =
		scientific_name_comprised;
	return species_comprised;
} /* freshwaterfish_species_comprised_new() */

LIST *freshwaterfish_get_species_comprised_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char scientific_name[ 128 ];
	char scientific_name_comprised[ 128 ];
	FILE *input_pipe;
	LIST *species_comprised_list;
	SPECIES_COMPRISED *species_comprised;
	char *select = "scientific_name,scientific_name_comprised";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=species_comprised	",
		 application_name,
		 select );


	species_comprised_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	scientific_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		piece(	scientific_name_comprised,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		species_comprised =
			freshwaterfish_species_comprised_new(
				strdup( scientific_name ),
				strdup( scientific_name_comprised ) );

		list_append_pointer(
			species_comprised_list,
			species_comprised );
	}

	pclose( input_pipe );
	return species_comprised_list;

} /* freshwaterfish_get_species_comprised_list() */

SPECIES_COMPRISED *freshwaterfish_get_species_comprised(
				LIST *species_comprised_list,
				char *scientific_name_comprised )
{
	SPECIES_COMPRISED *species_comprised = {0};

	if ( !list_rewind( species_comprised_list ) )
	{
		return (SPECIES_COMPRISED *)0;
	}

	do {
		species_comprised = list_get_pointer( species_comprised_list );

		if ( strcmp(	species_comprised->scientific_name_comprised,
				scientific_name_comprised ) == 0 )
		{
			return species_comprised;
		}
	} while( list_next( species_comprised_list ) );
	return (SPECIES_COMPRISED *)0;

} /* freshwaterfish_get_species_comprised() */

char *freshwaterfish_species_comprised_list_display(
					LIST *species_comprised_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	SPECIES_COMPRISED *species_comprised;
	int first_time = 1;

	if ( !list_rewind( species_comprised_list ) ) return "";

	*ptr = '\0';

	do {
		species_comprised = list_get_pointer( species_comprised_list );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf(	ptr,
				"%s=%s",
				species_comprised->scientific_name,
				species_comprised->scientific_name_comprised );
	} while( list_next( species_comprised_list ) );
	return strdup( buffer );
} /* freshwaterfish_species_comprised_list_display() */

int freshwaterfish_get_measurement_number(
				char *application_name,
				char *input_filename )
{
	int how_many;
	char sys_string[ 1024 ];

	sprintf( sys_string, "wc -l '%s'", input_filename );
	how_many = atoi( pipe2string( sys_string ) );

	sprintf( sys_string,
		 "reference_number.sh %s %d",
		 application_name,
		 how_many );
	return atoi( pipe2string( sys_string ) );
} /* freshwaterfish_get_measurement_number() */

char *freshwaterfish_library_get_species_category(
				char *scientific_name,
				char *application_name )
{
	static DICTIONARY *species_dictionary = {0};

	if ( !species_dictionary )
	{
		char sys_string[ 1024 ];
		char input_buffer[ 256 ];
		char scientific_name[ 128 ];
		char species_category[ 128 ];
		FILE *input_pipe;

		sprintf(sys_string,
		 	"get_folder_data	application=%s		"
		 	"			select=%s		"
		 	"			folder=species		",
		 	application_name,
		 	"scientific_name,species_category" );

		input_pipe = popen( sys_string, "r" );

		species_dictionary = dictionary_small();

		while( get_line( input_buffer, input_pipe ) )
		{
			piece(	scientific_name,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				0 );

			piece(	species_category,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				1 );

			dictionary_set_pointer(
				species_dictionary,
				strdup( scientific_name ),
				strdup( species_category ) );
		}
		pclose( input_pipe );
	}

	return dictionary_get_pointer( species_dictionary,
					scientific_name );

} /* freshwaterfish_library_get_species_category() */

char *freshwaterfish_get_clean_site( char *site )
{
	static char local_site[ 3 ];
	char *ptr = local_site;

	if ( *site == '0' ) site++;

	if ( *site ) *ptr++ = *site++;
	if ( *site ) *ptr++ = *site++;
	*ptr = '\0';
	return local_site;
} /* freshwaterfish_get_clean_site() */

