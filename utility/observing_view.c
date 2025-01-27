/*  observing_view.c */
/*  ----------------- */
/*  Tim Riley 	      */
/*  ----------------- */

#include <stdio.h>
#define MAX_FIELDS	30

typedef struct
{
	char *field_name;
	char *column_name;
} FIELD;

typedef struct
{
	FIELD field[ MAX_FIELDS ];
} TABLE;

TABLE observation = {
			{"percent_cloud_cover",		"Cloud"},
			{"wind_speed_mph",		"Wind Spd"},
			{"wind_direction_degrees",	"Wnd Dir"},
			{"current_speed_cps",		"Current"},
			{"current_direction_degrees",	"Curr Dir"},
			{"surface_water_temperature_c",	"Wat Temp"},
			{"surface_water_turbidity_ntu",	"Turbidty"},
			{"s_water_conductivity_msem_cm","Conduct"},
			{"surface_water_salinity_ppt",	"Salinity"},
			{"surface_water_ph",		"Water Ph"},
			{"s_water_dissolved_oxygen_mgl","Oxygen"},
			{(char *)0,			(char *)0}
			};

/* Prototypes */
/* ---------- */
void output_field( TABLE *observation,
		   int one_based, 
		   int output_column,
		   char *argv_0 );

void list_table( TABLE *observation );
int count_fields( TABLE *observation );

main( int argc, char **argv )
{
	int output_column;

	if ( argc < 2 )
	{
		fprintf( stderr,
			 "Usage: %s field_number|list [column]\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	if ( argc == 3 && strcmp( argv[ 2 ], "column" ) == 0 )
		output_column = 1;
	else
		output_column = 0;

	if ( strcmp( argv[ 1 ], "list" ) == 0 )
		list_table( &observation );
	else
	{
		output_field( &observation, 
			      atoi( argv[ 1 ] ), 
			      output_column,
			      argv[ 0 ] );
	}

	return 0;

} /* main() */


void output_field( TABLE *observation,
		   int one_based, 
		   int output_column,
		   char *argv_0 )
{
	int num_fields = count_fields( observation );

	if ( one_based < 1 || one_based > num_fields )
	{
		fprintf( stderr, 
			 "ERROR: %s bad field number: %d\n",
			 argv_0,
			 one_based );
		return;
	}

	if ( output_column )
		printf( "%s\n", 
			observation->field[ one_based - 1 ].column_name );
	else
		printf( "%s\n", 
			observation->field[ one_based - 1 ].field_name );

} /* output_field() */

void list_table( TABLE *observation )
{
	int i;
	int number_fields = count_fields( observation );

	for( i = 0; i < number_fields; i++ ) 
	{
		printf( "%.2d) %s\n", i + 1, 
			observation->field[ i ].field_name );
	}

} /* list_table() */


int count_fields( TABLE *observation )
{
	int i;

	for( i = 0; observation->field[ i ].field_name; i++ );
	return i;

} /* count_fields() */

