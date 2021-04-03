/* src_freshwaterfish/freshwaterfish_library.h */
/* ------------------------------------------- */

#ifndef FRESHWATERFISH_LIBRARY_H
#define FRESHWATERFISH_LIBRARY_H

#include "hash_table.h"

#define AGENCY_NPS			"national_park_service"
#define AGENCY_FIU			"florida_international_university"
#define UNKNOWN_SEX				"unknown"

#define NPS_REGION_SHARK_RIVER_SLOUGH		"shark_river_slough"
#define NPS_REGION_NORTHEAST_SHARK_SLOUGH	"northeast_shark_slough"

#define HABITAT_VISIT_DATE_PIECE			0
#define HABITAT_SITE_PIECE				4
#define HABITAT_PLOT_PIECE				5
#define HABITAT_TEMPERATURE_PIECE			21
#define HABITAT_OXYGEN_PIECE				22
#define HABITAT_CONDUCTIVITY_PIECE			23
#define HABITAT_THROW_NUMBER_PIECE			6
#define HABITAT_THROW_TIME_PIECE			9
#define HABITAT_LONG_GRID_PIECE				7
#define HABITAT_SHORT_GRID_PIECE			8
#define HABITAT_WATER_DEPTH_PIECE			10
#define HABITAT_NPS_CODE_PIECE				11
#define HABITAT_FIU_NON_FISH_OLD_CODE_PIECE		12
#define HABITAT_DENSITY_N_PIECE				13
#define HABITAT_DENSITY_O_PIECE				14
#define HABITAT_SURFACE_SUBMERGENT_PIECE		15
#define HABITAT_PLANT_HEIGHT_PIECE			16
#define HABITAT_EMERGENCE_PLANT_COVER_PIECE		17
#define HABITAT_TOTAL_PLANT_COVER_PIECE			18
#define HABITAT_PERIPHYTON_COVER_PIECE			19
#define HABITAT_PERIPHYTON_VOLUME_PIECE			20


#define MEASUREMENT_VISIT_DATE_PIECE			0
#define MEASUREMENT_SITE_PIECE				4
#define MEASUREMENT_PLOT_PIECE				5
#define MEASUREMENT_THROW_NUMBER_PIECE			6
#define MEASUREMENT_NPS_CODE_PIECE			7
#define MEASUREMENT_FIU_FISH_OLD_CODE_PIECE		8
#define MEASUREMENT_LENGTH_PIECE			9
#define MEASUREMENT_SEX_PIECE				10
#define MEASUREMENT_WEIGHT_PIECE			11
#define MEASUREMENT_COMMENTS_PIECE			12

#define SITE_VISIT_FIELD_LIST	"site,plot,region,visit_date,agency,temperature_celsius,disolved_oxygen_mg_per_liter,conductivity"
#define THROW_FIELD_LIST	"site,plot,region,visit_date,throw_number,long_grid_number,short_grid_number,throw_time,water_depth_cm,average_plant_height_cm,emergence_plant_cover_percent,total_plant_cover_percent,periphyton_cover_percent,periphyton_volume_milliliter"
#define LAB_SITE_VISIT_FIELD_LIST	"site,plot,region,visit_date,agency"
#define LAB_THROW_FIELD_LIST		"site,plot,region,visit_date,throw_number"
#define SURFACE_VEGETATION_FIELD_LIST "site,plot,region,visit_date,throw_number,scientific_name,density_alive_count,density_dead_count"
#define SUBMERGENT_VEGETATION_FIELD_LIST "site,plot,region,visit_date,throw_number,scientific_name,density_percent"
#define MEASUREMENT_FIELD_LIST	"site,plot,region,visit_date,throw_number,scientific_name,measurement_number,length_millimeters,sex,weight_grams,no_measurement_count,no_measurement_total_weight,comments"

#define SPECIES_SELECT "scientific_name,nps_code,fiu_fish_old_code,fiu_non_fish_old_code,fiu_new_code"
#define SURFACE_VEGETATION_SELECT "site,plot,region,visit_date,throw_number,scientific_name,density_alive_count,density_dead_count"
#define SUBMERGENT_VEGETATION_SELECT "site,plot,region,visit_date,throw_number,scientific_name,density_percent"
#define MEASUREMENT_SELECT "site,plot,region,visit_date,throw_number,measurement_number,scientific_name,length_millimeters,sex,weight_grams,no_measurement_count,no_measurement_total_weight,comments"

#define THROW_SELECT_TEMPLATE	"%s.site,%s.plot,%s.region,%s.visit_date,temperature_celsius,disolved_oxygen_mg_per_liter,conductivity,throw_number,long_grid_number,short_grid_number,throw_time,water_depth_cm,sediment,average_plant_height_cm,emergence_plant_cover_percent,total_plant_cover_percent,periphyton_cover_percent,periphyton_volume_milliliter"

#define JUSTIFY_COMMA_LIST	"right,right,right,right,right,right,right,right,right,right,right,right,right,right,right,right,right,right,right"
#define MEASUREMENT_JUSTIFY_COMMA_LIST	"left,left,left,left,left,left,left,right,left,right,right,right,right,right,right,right,right,right,right"

/* Structures */
/* ---------- */
typedef struct
{
	char *scientific_name;
	char *scientific_name_comprised;
} SPECIES_COMPRISED;

typedef struct
{
	LIST *throw_list;
	LIST *species_group_list;
} SPECIES_GROUP_WEIGHT;

typedef struct
{
	char *scientific_name;
	char *fiu_new_code;
	char *fiu_non_fish_old_code;
	double aggregation;
} SPECIES;

typedef struct
{
	char *site;
	char *plot;
	char *visit_date;
	int throw_number;
	HASH_TABLE *species_hash_table;
	HASH_TABLE *species_group_hash_table;
} THROW_WEIGHT_COUNT;

typedef struct
{
	LIST *throw_list;
	LIST *species_list;
	LIST *species_comprised_list;
} MEASUREMENT_COUNT;

typedef struct
{
	char *species_group_name;
	DICTIONARY *membership_dictionary;
	int count;
	double weight_sum;
} SPECIES_GROUP;

typedef struct
{
	int measurement_number;
	char *scientific_name;
	char *length_millimeters;
	char *sex;
	char *weight_grams;
	char *no_measurement_count;
	char *no_measurement_total_weight;
	char *comments;
} MEASUREMENT;

typedef struct
{
	char *scientific_name;
	char *density_alive_count;
	char *density_dead_count;
} SURFACE_VEGETATION;

typedef struct
{
	char *scientific_name;
	char *density_percent;
} SUBMERGENT_VEGETATION;

typedef struct
{
	char site[ 16 ];
	char plot[ 2 ];
	char region[ 128 ];
	char visit_date[ 16 ];
	char temperature_celsius[ 16 ];
	char disolved_oxygen_mg_per_liter[ 16 ];
	char conductivity[ 16 ];
	int throw_number;
	int long_grid_number;
	int short_grid_number;
	char throw_time[ 5 ];
	char water_depth_cm[ 16 ];
	char sediment[ 16 ];
	char average_plant_height_cm[ 16 ];
	char emergence_plant_cover_percent[ 16 ];
	char total_plant_cover_percent[ 16 ];
	char periphyton_cover_percent[ 16 ];
	char periphyton_volume_milliliter[ 16 ];
	LIST *surface_vegetation_list;
	LIST *submergent_vegetation_list;
	LIST *measurement_list;
} THROW;

/* Prototypes */
/* ---------- */
LIST *freshwaterfish_get_species_record_list(
				char *application_name );

char *freshwaterfish_library_get_scientific_name(
				char *nps_code,
				char *fiu_fish_old_code,
				char *fiu_non_fish_old_code,
				char *fiu_new_code,
				char *application_name );

char *freshwaterfish_library_get_species_category(
				char *scientific_name,
				char *application_name );

DICTIONARY *freshwaterfish_library_get_species_code_dictionary(
				LIST *species_record_list,
				int code_piece_offset );

char *freshwaterfish_get_visit_date_international(
				char *visit_date_compressed );

char *freshwaterfish_get_visit_date_compressed(
				char *visit_date_international );

void freshwaterfish_library_pad_time_four(
				char *throw_time_string );

boolean freshwaterfish_get_throw_between_dates(
				THROW *throw,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *agency );

void freshwaterfish_parse_throw(
				THROW *throw,
				char *input_buffer );
int freshwaterfish_get_visit_year(
				char *visit_date );
int freshwaterfish_get_visit_month(
				char *visit_date );
int freshwaterfish_get_visit_day(
				char *visit_date );

SURFACE_VEGETATION *freshwaterfish_surface_vegetation_new(
				char *scientific_name,
				char *density_alive_count,
				char *density_dead_count );

SUBMERGENT_VEGETATION *freshwaterfish_submergent_vegetation_new(
				char *scientific_name,
				char *density_percent );

MEASUREMENT *freshwaterfish_measurement_new(
				int measurement_number,
				char *scientific_name,
				char *length_millimeters,
				char *sex,
				char *weight_grams,
				char *no_measurement_count,
				char *no_measurement_total_weight,
				char *comments );

LIST *freshwaterfish_get_surface_vegetation_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

LIST *freshwaterfish_get_surface_vegetation_record_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string );

LIST *freshwaterfish_get_submergent_vegetation_record_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string );

LIST *freshwaterfish_get_submergent_vegetation_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

char *freshwaterfish_get_nps_code(
				char *application_name,
				char *scientific_name );

char *freshwaterfish_get_fiu_non_fish_old_code(
				char *application_name,
				char *scientific_name );

char *freshwaterfish_get_fiu_new_code(
				char *application_name,
				char *scientific_name );

LIST *freshwaterfish_get_measurement_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

LIST *freshwaterfish_get_measurement_record_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string );

char *freshwaterfish_get_fiu_fish_old_code(
				char* application_name,
				char *scientific_name );

char *freshwaterfish_with_fiu_code_get_sex(
				char *application_name,
				char *fiu_sex_code );

char *freshwaterfish_get_sex(
				char *application_name,
				char *sex_alpha_code );

char *freshwaterfish_get_sex_fiu_code(
				char *application_name,
				char *sex );

char *freshwaterfish_get_sex_alpha_code(
				char *application_name,
				char *sex );

char *freshwaterfish_get_nps_region(
				char *site );

boolean freshwaterfish_exists_region(
				char *application_name,
				char *region );

boolean freshwaterfish_exists_agency(
				char *application_name,
				char *agency );

boolean freshwaterfish_exists_sex(
				char *application_name,
				char *sex );

MEASUREMENT_COUNT *freshwaterfish_measurement_count_new(
				char *application_name,
				LIST *species_group_name_list );

THROW_WEIGHT_COUNT *freshwaterfish_throw_weight_count_new(
				char *site,
				char *plot,
				char *visit_date,
				int throw_number );

LIST *freshwaterfish_get_species_list(
				char *application_name,
				LIST *species_group_name_list,
				LIST *species_comprised_list );

SPECIES *freshwaterfish_species_new(
				char *scientific_name );

boolean freshwaterfish_in_species_membership(
				char *application_name,
				LIST *species_group_name_list,
				char *scientific_name );

boolean freshwaterfish_species_in_species_membership(
				char *application_name,
				char *species_group,
				char *scientific_name );

LIST *freshwaterfish_get_species_group_list(
				char *application_name );

SPECIES_GROUP *freshwaterfish_species_group_new(
				char *application_name,
				char *species_group_name,
				boolean with_membership_dictionary );

SPECIES_COMPRISED *freshwaterfish_species_comprised_new(
				char *scientific_name,
				char *scientific_name_comprised );

DICTIONARY *freshwaterfish_get_membership_dictionary(
				char *application_name,
				char *species_category_name );

LIST *freshwaterfish_get_species_group_name_list(
				char *application_name );

LIST *freshwaterfish_get_species_throw_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *select_shell_script,
				LIST *species_comprised_list );

LIST *freshwaterfish_get_species_group_throw_list(
				char *application_name,
				char *begin_date_string,
				char *end_date_string );

char *freshwaterfish_get_species_category_where_in_clause(
				char *application_name,
				LIST *species_category_name_list );

SPECIES_GROUP_WEIGHT *freshwaterfish_species_group_weight_new(
				char *application_name,
				LIST *species_group_name_list );

LIST *freshwaterfish_get_scientific_name_comprised_list(
				LIST *species_comprised_list );

LIST *freshwaterfish_get_species_comprised_list(
				char *application_name );

SPECIES_COMPRISED *freshwaterfish_get_species_comprised(
				LIST *species_comprised_list,
				char *scientific_name_comprised );

char *freshwaterfish_species_comprised_list_display(
				LIST *species_comprised_list );

char *freshwaterfish_get_region(char *application_name,
				char *region_code );

int freshwaterfish_get_measurement_number(
				char *application_name,
				char *input_filename );

char *freshwaterfish_library_get_species_category(
				char *scientific_name,
				char *application_name );

char *freshwaterfish_get_clean_site(
				char *site );

#endif
