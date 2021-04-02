/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/creel.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef CREEL_H
#define CREEL_H

/* Includes */
/* -------- */
#include "list.h"
#include "hash_table.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define PERMITS_SELECT		"permit_code,"			\
				"guide_angler_name,"		\
				"issued_date,"			\
				"expiration_date"

#define CATCHES_SELECT		"family,"			\
				"genus,"			\
				"species,"			\
				"kept_count,"			\
				"released_count"

#define FISHING_TRIPS_SELECT	"census_date,"			\
				"interview_location,"		\
				"interview_number,"		\
				"fishing_purpose,"		\
				"trip_hours,"			\
				"number_of_people_fishing,"	\
				"hours_fishing,"		\
				"genus,"			\
				"species_preferred,"		\
				"family,"			\
				"location_area,"		\
				"location_zone,"		\
				"interview_time,"		\
				"fishing_party_composition,"	\
				"trip_origin_location,"		\
				"recreational_angler_reside,"	\
				"permit_code,"			\
				"fishing_area,"			\
				"last_person_validating,"	\
				"login_name,"			\
				"guide_angler_submission_yn,"	\
				"last_changed_by,"		\
				"validation_date"

/* Structures */
/* ---------- */
typedef struct
{
	char *permit_code;
	char *guide_angler_name;
	char *issued_date;
	char *expiration_date;
} CREEL_PERMITS;

typedef struct
{
	char *family;
	char *genus;
	char *species;
	int kept_count;
	int released_count;
} CREEL_CATCH_MEASUREMENTS;

typedef struct
{
	char *family;
	char *genus;
	char *species;
	int kept_count;
	int released_count;
} CREEL_CATCHES;

typedef struct
{
	int interview_number;
	char *interview_time;
	char *trip_origin_location;
	char *family;
	char *genus;
	char *species_preferred;
	char *fishing_area;
	char *fishing_party_composition;
	int trip_hours;
	int hours_fishing;
	int number_of_people_fishing;
	char *permit_code;
	char *recreational_angler_reside;
	LIST *catches_list;
} CREEL_FISHING_TRIPS;

typedef struct
{
	char *fishing_purpose;
	char *census_date;
	char *interview_location;
	char *researcher;
	LIST *fishing_trips_list;
} CREEL_CENSUS;

typedef struct
{
	LIST *creel_census_list;
	HASH_TABLE *creel_census_hash_table;
	HASH_TABLE *fishing_trips_hash_table;
	HASH_TABLE *catches_hash_table;
	LIST *creel_permits_list;
	HASH_TABLE *permits_hash_table;
} CREEL;

/* Prototypes */
/* ---------- */
CREEL *creel_new(	char *application_name,
			char *begin_census_date,
			char *end_census_date,
			char *fishing_purpose );

HASH_TABLE *creel_census_hash_table(
			LIST **creel_census_list,
			char *application_name,
			char *begin_census_date,
			char *end_census_date,
			char *fishing_purpose );

HASH_TABLE *creel_fishing_trips_hash_table(
			char *application_name,
			char *begin_census_date,
			char *end_census_date,
			char *fishing_purpose );

HASH_TABLE *creel_catches_hash_table(
			char *application_name,
			char *begin_census_date,
			char *end_census_date,
			char *fishing_purpose );

CREEL_CENSUS *creel_census_new(
			char *fishing_purpose,
			char *census_date,
			char *interview_location );

CREEL_FISHING_TRIPS *creel_fishing_trips_calloc(
			void );

CREEL_FISHING_TRIPS *creel_fishing_trips_new(
			int interview_number );

CREEL_CATCHES *creel_catches_new(
			char *family,
			char *genus,
			char *species );

char *creel_census_key(	char *fishing_purpose,
			char *census_date,
			char *interview_location );

char *creel_fishing_trips_key(
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			int interview_number );

char *creel_catches_key(
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			int interview_number,
			char *family,
			char *genus,
			char *species );

CREEL_CENSUS *creel_census_hash_table_fetch(
			HASH_TABLE *creel_census_hash_table,
			char *fishing_purpose,
			char *census_date,
			char *interview_location );

CREEL_FISHING_TRIPS *creel_fishing_trips_hash_table_fetch(
			HASH_TABLE *fishing_trips_hash_table,
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			int interview_number );

CREEL_CATCHES *creel_catches_fetch(
			HASH_TABLE *catches_hash_table,
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			int interview_number,
			char *family,
			char *genus,
			char *species );

void creel_append_fishing_trips(
			HASH_TABLE *creel_census_hash_table,
			HASH_TABLE *fishing_trips_hash_table );

void creel_append_catches(
			HASH_TABLE *fishing_trips_hash_table,
			HASH_TABLE *catches_hash_table );

CREEL_FISHING_TRIPS *creel_fishing_trips_fetch(
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			int interview_number,
			boolean fetch_catches_list,
			boolean fetch_catch_measurements_list,
			boolean fetch_permit );

/* Returns static memory */
/* --------------------- */
char *creel_fishing_trips_primary_where(
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			int interview_number );

char *creel_fishing_trips_system_string(
			char *where );

CREEL_PERMITS *creel_permits_calloc(
			void );

CREEL_PERMITS *creel_permits_parse(
			char *input );

char *creel_permits_system_string(
			char *where );

LIST *creel_permits_list_fetch(
			char *where );

LIST *creel_permits_system_list(
			char *system_string );

HASH_TABLE *creel_permits_hash_table_fetch(
			char *where );

/* Returns static memory */
/* --------------------- */
char *creel_permits_hash_table_key(
			char *permit_code );

CREEL_PERMITS *creel_permits_hash_table_seek(
			char *permit_code,
			HASH_TABLE *permit_hash_table );

/* Returns static memory */
/* --------------------- */
char *creel_hash_table_permit_code(
			char *permit_code,
			HASH_TABLE *permit_code_hash_table,
			boolean execute );

#endif
