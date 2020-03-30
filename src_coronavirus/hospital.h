/* --------------------------------------------- */
/* $APPASERVER_HOME/src_coronavirus/hospital.h	 */
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
	char *date_time_greenwich;
	int ventilator_count;
} CURRENT_VENTILATOR_COUNT;

typedef struct
{
	char *date_time_greenwich;
	int non_coronavirus_current_patient_count;
	int coronavirus_admitted_todate;
	int coronavirus_released_todate;
	int coronavirus_martality_todate;
	int coronavirus_current_patient_count;
	int coronavirus_patient_daily_change;
	int coronavirus_released_daily_change;
	int coronavirus_mortality_daily_change;
} CURRENT_PATIENT_COUNT;

typedef struct
{
	char *date_time_greenwich;
	int regular_bed_occupied_count;
	int ICU_bed_occupied_count;
} CURRENT_BED_USAGE;

typedef struct
{
	char *date_time_greenwich;
	int regular_bed_capacity;
	int ICU_bed_capacity;
} CURRENT_BED_CAPACITY;

typedef struct
{
	char *hospital_name;
	char *street_address;
	char *city;
	char *state_code;

	int non_coronavirus_current_patient_count;
	boolean non_coronavirus_current_patient_count_isnull;

	int coronavirus_current_patient_count;
	boolean coronavirus_current_patient_count_isnull;

	int ventilator_count;
	boolean ventilator_count_isnull;

	int coronavirus_patients_without_ventilators;
	boolean coronavirus_patients_without_ventilators_isnull;

	int coronavirus_admitted_todate;
	boolean coronavirus_admitted_todate_isnull;

	int coronavirus_released_todate;
	boolean coronavirus_released_todate_isnull;

	int coronavirus_mortality_todate;
	boolean coronavirus_mortality_todate_isnull;

	int coronavirus_admitted_daily_change;
	boolean coronavirus_admitted_daily_change_isnull;

	boolean coronavirus_released_daily_change_isnull;
	int coronavirus_released_daily_change;

	boolean coronavirus_mortality_daily_change_isnull;
	int coronavirus_mortality_daily_change;

	int regular_bed_capacity;
	boolean regular_bed_capacity_isnull;

	int regular_bed_occupied_count;
	boolean regular_bed_occupied_count_isnull;

	int regular_bed_occupied_percent;
	boolean regular_bed_occupied_percent_isnull;

	int ICU_bed_capacity;
	boolean ICU_bed_capacity_isnull;

	int ICU_bed_occupied_count;
	boolean ICU_bed_occupied_count_isnull;

	int ICU_bed_occupied_percent;
	boolean ICU_bed_occupied_percent_isnull;

	char *hospital_type;
	char *ownership_structure;
	boolean active;
	boolean helipad;
	char *latitude;
	char *longitude;
	char *zip_code;
	char *zip4;
	char *telephone;
	char *hospital_website;

	LIST *current_patient_count_list;
	LIST *current_bed_usage_list;
	LIST *current_bed_capacity_list;
	LIST *current_ventilator_count_list;
} HOSPITAL;

/* ========== */
/* Operations */
/* ========== */

/* CURRENT_PATIENT_COUNT */
/* --------------------- */
CURRENT_PATIENT_COUNT *hospital_current_patient_count_new(
				void );

CURRENT_PATIENT_COUNT *hospital_current_patient_count_parse(
				char *input_line );

char *hospital_current_patient_count_select(
				void );

LIST *hospital_current_patient_count_list(
				char *application_name,
				char *hospital_name,
				char *street_address );

boolean hospital_current_patient_count_set_last(
				LIST *current_patient_count_list );

int hospital_coronavirus_current_patient_count(
				int coronovirus_admitted_todate,
				int coronovirus_released_todate,
				int coronovirus_mortality_todate );

int hospital_coronavirus_patient_daily_change(
				CURRENT_PATIENT_COUNT *last_patient_count,
				CURRENT_PATIENT_COUNT *prior_patient_count );

int hospital_coronavirus_released_daily_change(
				CURRENT_PATIENT_COUNT *last_patient_count,
				CURRENT_PATIENT_COUNT *prior_patient_count );

int hospital_coronavirus_mortality_daily_change(
				CURRENT_PATIENT_COUNT *last_patient_count,
				CURRENT_PATIENT_COUNT *prior_patient_count );

/* CURRENT_VENTILATOR_COUNT */
/* ------------------------ */
CURRENT_VENTILATOR_COUNT *hospital_current_ventilator_count_new(
				void );

CURRENT_VENTILATOR_COUNT *hospital_current_ventilator_count_parse(
				char *input_line );

char *hospital_current_ventilator_count_select(
				void );

LIST *hospital_current_ventilator_count_list(
				char *application_name,
				char *hospital_name,
				char *street_address );

boolean hospital_current_ventilator_count_set_last(
				LIST *current_ventilator_count_list );

int hospital_current_ventilator_count(
			CURRENT_VENTILATOR_COUNT *last_ventilator_count );

/* CURRENT_BED_USAGE */
/* ----------------- */
CURRENT_BED_USAGE *hospital_current_bed_usage_new(
				void );

CURRENT_BED_USAGE *hospital_current_bed_usage_parse(
				char *input_line );

char *hospital_current_bed_usage_select(
				void );

LIST *hospital_current_bed_usage_list(
				char *application_name,
				char *hospital_name,
				char *street_address );

boolean hospital_current_bed_usage_set_last(
				LIST *current_bed_usage_list );

int hospital_regular_bed_occupied_count(
				CURRENT_BED_USAGE *last_bed_usage );

int hospital_ICU_bed_occupied_count(
				CURRENT_BED_USAGE *last_bed_usage );

/* CURRENT_BED_CAPACITY */
/* -------------------- */
CURRENT_BED_CAPACITY *hospital_current_bed_capacity_new(
				void );

CURRENT_BED_CAPACITY *hospital_current_bed_capacity_parse(
				char *input_line );

char *hospital_current_bed_capacity_select(
				void );

LIST *hospital_current_bed_capacity_list(
				char *application_name,
				char *hospital_name,
				char *street_address );

boolean hospital_current_bed_capacity_set_last(
				LIST *current_bed_capacity_list );

int hospital_regular_bed_capacity(
				CURRENT_BED_CAPACITY *current_bed_capacity );

int hospital_ICU_bed_capacity(
				CURRENT_BED_CAPACITY *current_bed_capacity );

/* HOSPITAL */
/* -------- */
HOSPITAL *hospital_new(		void );

/* Returns program memory. */
/* ----------------------- */
char *hospital_select(		void );

HOSPITAL *hospital_fetch(	char *application_name,
				char *hospital_name,
				char *street_address );

HOSPITAL *hospital_parse(	char *input_line );

int hospital_regular_bed_occupied_percent(
				boolean *isnull,
				int regular_bed_capacity,
				LIST *current_bed_usage_list );

int hospital_ICU_bed_occupied_percent(
				boolean *isnull,
				int ICU_bed_capacity,
				LIST *current_bed_usage_list );

void hospital_update(
	char *application_name,
	char *hospital_name,
	char *street_address,

	int non_coronavirus_current_patient_count,
	boolean non_coronavirus_current_patient_count_isnull,

	int coronavirus_current_patient_count,
	boolean coronavirus_current_patient_count_isnull,

	int ventilator_count,
	boolean ventilator_count_isnull,

	int coronavirus_patients_without_ventilators,
	boolean coronavirus_patients_without_ventilators_isnull,

	int coronavirus_admitted_to_date,
	boolean coronavirus_admitted_to_date_isnull,

	int coronavirus_released_to_date,
	boolean coronavirus_released_to_date_isnull,

	int coronavirus_mortality_to_date,
	boolean coronavirus_mortality_to_date_isnull,

	int regular_bed_capacity,
	boolean regular_bed_capacity_isnull,

	int regular_bed_occupied_count,
	boolean regular_bed_occupied_count_isnull,

	int regular_bed_occupied_percent,
	boolean regular_bed_occupied_percent_isnull,

	int ICU_bed_capacity,
	boolean ICU_bed_capacity_isnull,

	int ICU_bed_occupied_count,
	boolean ICU_bed_occupied_count_isnull,

	int ICU_bed_occupied_percent,
	boolean ICU_bed_occupied_percent_isnull );

/* Returns heap memory. */
/* -------------------- */
char *hospital_where(		char *hospital_name,
				char *street_address );

int hospital_coronavirus_patients_without_ventilators(
				int coronavirus_current_patient_count,
				int ventilator_count );

/* Returns program memory. */
/* ----------------------- */
char *hospital_escape_name(	char *hospital_name );

int hospital_admitted_delta(
				LIST *current_patient_count_list );

int hospital_released_delta(
				LIST *current_patient_count_list );

int hospital_mortality_delta(
				LIST *current_patient_count_list );

int hospital_coronavirus_admitted_daily_change(
				CURRENT_PATIENT_COUNT *last,
				CURRENT_PATIENT_COUNT *prior );

#endif

