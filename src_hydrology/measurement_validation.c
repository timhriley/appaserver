/* $APPASERVER_HOME/src_hydrology/measurement_validation.c */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "measurement_validation.h"

void measurement_validation_insert_into_manual_validation_event(
			char *application_name,
			char *login_name,
			char *station,
			char *datatype,
			char *begin_measurement_date,
			char *end_measurement_date,
			char *validation_date,
			char *validation_time,
			char *validation_process )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name =
		get_table_name(
			application_name, 
			"manual_validation_event" );

	sprintf( sys_string,
"echo \"insert into %s (station,			 "
"			datatype,			 "
"			login_name,			 "
"			validation_date,		 "
"			validation_time,		 "
"			begin_measurement_date,		 "
"			end_measurement_date,		 "
"			validation_process )		 "
"values ('%s','%s','%s','%s','%s','%s','%s','%s');\"	|"
"sql 2>&1						|"
"html_paragraph_wrapper					 ",
		 table_name,
		 station,
		 datatype,
		 login_name,
		 validation_date,
		 validation_time,
		 begin_measurement_date,
		 end_measurement_date,
		 validation_process );

	if ( system( sys_string ) ){};
}

int measurement_validation_update_measurement(
			char *application_name,
			char *login_name,
			char *station,
			char *datatype,
			char *validation_date,
			char *begin_measurement_date,
			char *end_measurement_date )
{
	char where[ 4096 ];
	char sys_string[ 4096 ];
	char *table_name;
	char last_validation_date_set_string[ 128 ];
	char last_person_validating_set_string[ 128 ];
	char *validation_process;
	int return_count;

	table_name =
		get_table_name(
			application_name, 
			"measurement" );

	if ( !validation_date || !*validation_date )
	{
		strcpy( last_validation_date_set_string, "= null" );

		sprintf(	last_person_validating_set_string,
				"= '%s'",
				login_name );

		validation_process =
			MEASUREMENT_VALIDATION_UNVALIDATION_PROCESS;
	}
	else
	{
		sprintf( last_validation_date_set_string,
			 "= '%s'",
			 validation_date );
		sprintf( last_person_validating_set_string,
			 "= '%s'",
			 login_name );
		validation_process =
			MEASUREMENT_VALIDATION_VALIDATION_PROCESS;
	}

	sprintf( where,
		 "station = '%s' and				"
		 "datatype = '%s' and				"
		 "measurement_date between '%s' and '%s'	",
		 station,
		 datatype,
		 begin_measurement_date,
		 end_measurement_date );

	sprintf( sys_string,
		 "echo \"select count(*) from %s where %s;\" | sql.e",
		 table_name,
		 where );

	return_count = atoi( pipe2string( sys_string ) );

	sprintf( sys_string,
"echo \"update %s set last_validation_date %s,		 "
"		      last_person_validating %s,	 "
"		      last_validation_process = '%s'	 "
"	where %s;\"				 	|"
"sql.e 2>&1						|"
"html_paragraph_wrapper					 ",
		 table_name,
		 last_validation_date_set_string,
		 last_person_validating_set_string,
		 validation_process,
		 where );

	appaserver_output_error_message(
		application_name, sys_string, login_name );

	system( sys_string );

	return return_count;

}

