/* ---------------------------------------------------	*/
/* src_benthic/sweep.h					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef SWEEP_H
#define SWEEP_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define SWEEP_TOTAL_MINUTES_KEY		"sum_minutes"

/* Structures */
/* ---------- */
typedef struct
{
	int replicate_number;
	int sweep_number;
	char *pick_date;
	char *quality_control_date;
	int wash_time_minutes;
	int pick_time_minutes;
	int quality_control_time_minutes;
	int sort_minutes;
	int identify_penaeid_minutes;
	int identify_fish_minutes;
	int identify_caridean_minutes;
	int identify_crabs_minutes;
	int total_minutes;
	char *pick_researcher;
	char *quality_control_researcher;
	char *laboratory_researcher;
	char *no_animals_found_yn;
	char *tag_missing_yn;
	char *notes;
} SWEEP;

typedef struct
{
	LIST *non_primary_key_list;
	LIST *sweep_list;
} SWEEP_LIST;

/* Prototypes */
/* ---------- */
SWEEP_LIST *sweep_list_new(	char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				int site_number,
				int replicate_number );

SWEEP *sweep_new(		int sweep_number );

LIST *sweep_sampling_point_sweep_list(
				char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				int site_number,
				int replicate_number,
				LIST *non_primary_key_list );

LIST *sweep_non_primary_key_list(
				boolean with_total_minutes );

int sweep_non_primary_key_list_piece_offset(
				LIST *non_primary_key_list,
				char *attribute_name );

char *sweep_minutes_sum_expression(
				void );

#endif
