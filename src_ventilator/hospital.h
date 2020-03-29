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
	char *date_time_greenwich;
	int current_ventilator_count;
} CURRENT_VENTILATOR_COUNT;

typedef struct
{
	char *date_time_greenwich;
	int non_coronavirus_current_patient_count;
	int coronavirus_patient_count_todate;
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
	char *zip_code;
	char *zip4;
	char *telephone;
	boolean active;

	boolean non_coronavirus_current_patient_count_isnull;
	int non_coronavirus_current_patient_count;

	boolean coronavirus_current_patient_count_isnull;
	int coronavirus_current_patient_count;

	boolean ventilator_count_isnull;
	int ventilator_count;

	boolean coronavirus_patients_without_ventilators_isnull;
	int coronavirus_patients_without_ventilators;

	boolean coronavirus_released_todate_isnull;
	int coronavirus_released_todate;

	boolean coronavirus_mortality_todate_isnull;
	int coronavirus_mortality_todate;

	boolean coronavirus_patient_daily_change_isnull;
	int coronavirus_patient_daily_change;

	boolean coronavirus_released_daily_change_isnull;
	int coronavirus_released_daily_change;

	boolean coronavirus_mortality_daily_change_isnull;
	int coronavirus_mortality_daily_change;

	boolean regular_bed_capacity_isnull;
	int regular_bed_capacity;

	boolean regular_bed_occupied_count_isnull;
	int regular_bed_occupied_count;

	boolean regular_bed_occupied_percent_isnull;
	int regular_bed_occupied_percent;

	boolean ICU_bed_capacity_isnull;
	int ICU_bed_capacity;

	boolean ICU_bed_occupied_count_isnull;
	int ICU_bed_occupied_count;

	boolean ICU_bed_occupied_percent_isnull;
	int ICU_bed_occupied_percent;

	char *hospital_type;
	char *owner_type;
	boolean helipad;
	char *latitude;
	char *longitude;
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

/* CURRENT_BED_CAPACITY */
/* -------------------- */
CURRENT_BED_CAPACITY *hospital_current_bed_capacity_new(
				void );

CURRENT_BED_CAPACITY *hospital_current_bed_capacity_parse(
				char *input_line );

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

int hospital_coronavirus_patients_without_ventilators(
				boolean *isnull,
				int regluar_bed_capacity,
				int ICU_bed_capacity,
				LIST *current_ventilator_count_list );

/* Returns program memory. */
/* ----------------------- */
char *hospital_escape_name(	char *hospital_name );

#endif

