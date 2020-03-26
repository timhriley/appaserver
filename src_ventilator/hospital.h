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
	char *hospital_name;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *zip4;
	char *telephone;
	int hospital_bed_capacity;
	int ICU_bed_capacity;
	boolean active;
	int hospital_bed_capacity_percent;
	int ICU_bed_capacity_percent;
	int beds_without_ventilators;
	char *hospital_type;
	char *owner_type;
	boolan helipad;
	char *latitude;
	char *longitude;
	int population_thousands;
	char *hospital_website;
} HOSPITAL;

/* Operations */
/* ---------- */
HOSPITAL *hospital_new(		char *hospital_name,
				char *street_address );

int hospital_bed_capacity_percent(
				boolean *isnull,
				int hospital_bed_capacity,
				LIST *current_bed_usage_list );

int hospital_ICU_bed_capacity_percent(
				boolean *isnull,
				int hospital_ICU_bed_capacity,
				LIST *current_bed_usage_list );

HOSPITAL *hospital_fetch(	char *application_name,
				char *hospital_name,
				char *street_address );

void hospital_update(		char *application_name,
				char *hospital_name,
				char *street_address,
				boolean hospital_bed_capacity_percent_isnull,
				int hospital_bed_capacity_percent,
				boolean ICU_bed_capacity_percent_isnull,
				int ICU_bed_capacity_percent,
				boolean beds_without_ventilators_isnull,
				int beds_without_ventilators );

#endif

