/* --------------------------------------------- */
/* $APPASERVER_HOME/src_ventilator/hospital.h	 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef HOSPITAL_H
#define HOSPITAL_H

#include "list.h"
#include "boolean.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *date_time_zulu;
	int current_ventilator_count;
} CURRENT_VENTILATOR_COUNT;

typedef struct
{
	char *date_time_zulu;
	int ventilator_bed_occupied_count;
	int ICU_bed_occupied_count;
} CURRENT_BED_USAGE;

typedef struct
{
	char *hospital_name;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *zip4;
	char *telephone;
	int ventilator_bed_capacity;
	int ICU_bed_capacity;
	boolean active;

	boolean current_ventilator_ICU_bed_occupied_count_isnull;
	int current_ventilator_ICU_bed_occupied_count;

	boolean current_ventilator_count_isnull;
	int current_ventilator_count;

	boolean necessary_beds_without_ventilators_isnull;
	int necessary_beds_without_ventilators;

	boolean ventilator_bed_occupied_percent_isnull;
	int ventilator_bed_occupied_percent;

	boolean ICU_bed_occupied_percent_isnull;
	int ICU_bed_occupied_percent;

	boolean current_ventilator_bed_occupied_count_isnull;
	int current_ventilator_bed_occupied_count;

	boolean current_ICU_bed_occupied_count_isnull;
	int current_ICU_bed_occupied_count;

	char *hospital_type;
	char *owner_type;
	boolean helipad;
	char *latitude;
	char *longitude;
	int population_thousands;
	char *hospital_website;
	LIST *current_bed_usage_list;
	LIST *current_ventilator_count_list;
} HOSPITAL;

/* Operations */
/* ---------- */
char *hospital_current_ventilator_count_select(
				void );

LIST *hospital_current_ventilator_count_list(
				char *application_name,
				char *hospital_name,
				char *street_address );

CURRENT_VENTILATOR_COUNT *hospital_current_ventilator_count_new(
				void );

CURRENT_VENTILATOR_COUNT *hospital_current_ventilator_count_parse(
				char *input_line );

char *hospital_current_bed_usage_select(
				void );

LIST *hospital_current_bed_usage_list(
				char *application_name,
				char *hospital_name,
				char *street_address );

CURRENT_BED_USAGE *hospital_current_bed_usage_new(
				void );

CURRENT_BED_USAGE *hospital_current_bed_usage_parse(
				char *input_line );

HOSPITAL *hospital_parse(	char *input_line );

HOSPITAL *hospital_new(		void );

int hospital_ventilator_bed_occupied_percent(
				boolean *isnull,
				int ventilator_bed_capacity,
				LIST *current_bed_usage_list );

int hospital_ICU_bed_occupied_percent(
				boolean *isnull,
				int ICU_bed_capacity,
				LIST *current_bed_usage_list );

HOSPITAL *hospital_parse(	char *input_line );

/* Returns program memory. */
/* ----------------------- */
char *hospital_select(		void );

HOSPITAL *hospital_fetch(	char *application_name,
				char *hospital_name,
				char *street_address );

void hospital_update(
	char *application_name,
	char *hospital_name,
	char *street_address,

	boolean current_ventilator_ICU_bed_occupied_count_isnull,
	int current_ventilator_ICU_bed_occupied_count,

	boolean current_ventilator_count_isnull,
	int current_ventilator_count,

	boolean necessary_beds_without_ventilators_isnull,
	int necessary_beds_without_ventilators,

	boolean ventilator_bed_occupied_percent_isnull,
	int ventilator_bed_occupied_percent,

	boolean ICU_bed_occupied_percent_isnull,
	int ICU_bed_occupied_percent,

	boolean current_ventilator_bed_occupied_count_isnull,
	int current_ventilator_bed_occupied_count,

	boolean current_ICU_bed_occupied_count_isnull,
	int current_ICU_bed_occupied_count );

/* Returns heap memory. */
/* -------------------- */
char *hospital_where(		char *hospital_name,
				char *street_address );

int hospital_necessary_beds_without_ventilators(
				boolean *isnull,
				int ventilator_bed_capacity,
				int ICU_bed_capacity,
				LIST *current_ventilator_count_list );

/* Returns program memory. */
/* ----------------------- */
char *hospital_escape_name(	char *hospital_name );

#endif

