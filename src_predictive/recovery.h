/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/recovery.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RECOVERY_H
#define RECOVERY_H

#include "list.h"
#include "boolean.h"
#include "String.h"
#include "html.h"
#include "date.h"

#define RECOVERY_TABLE				"cost_recovery"

#define RECOVERY_SELECT				"asset_name,"		\
						"serial_label,"		\
						"tax_year,"		\
						"recovery_rate,"	\
						"recovery_amount"

#define RECOVERY_METHOD_STRAIGHT_LINE		"straight_line"
#define RECOVERY_METHOD_ACCELERATED		"accelerated"
#define RECOVERY_CONVENTION_HALF_YEAR		"half_year"
#define RECOVERY_CONVENTION_MID_MONTH		"mid_month"
#define RECOVERY_CONVENTION_MID_QUARTER		"mid_quarter"
#define RECOVERY_SYSTEM_STATUTORY	  	"statutory"
#define RECOVERY_SYSTEM_ALTERNATIVE	  	"alternative"

typedef struct
{
	double rate;
	double amount;
} RECOVERY_STRAIGHT_LINE;

/* Usage */
/* ----- */
RECOVERY_STRAIGHT_LINE *recovery_straight_line_new(
		double cost_basis,
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_convention,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

/* Process */
/* ------- */
RECOVERY_STRAIGHT_LINE *recovery_straight_line_calloc(
		void );

double recovery_straight_line_half_year_rate(
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

double recovery_straight_line_mid_month_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

double recovery_straight_line_mid_quarter_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

double recovery_straight_line_amount(
		double cost_basis,
		double rate );

typedef struct
{
	double rate;
	double amount;
} RECOVERY_ACCELERATED;

/* Usage */
/* ----- */
RECOVERY_ACCELERATED *recovery_accelerated_new(
		double cost_basis,
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_convention,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

/* Process */
/* ------- */
RECOVERY_ACCELERATED *recovery_accelerated_calloc(
		void );

double recovery_accelerated_half_year_rate(
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

double recovery_accelerated_mid_month_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

double recovery_accelerated_mid_quarter_rate(
		int service_placement_month,
		int disposal_month,
		char *cost_recovery_system,
		int recovery_year,
		double recovery_class_year );

typedef struct
{
	char *asset_name;
	char *serial_label;
	int tax_year;
	double cost_basis;
	DATE *service_placement_date;
	int service_placement_month;
	int service_placement_year;
	DATE *disposal_date;
	int disposal_month;
	int disposal_year;
	int year;
	double class_year;
	RECOVERY_STRAIGHT_LINE *recovery_straight_line;
	RECOVERY_ACCELERATED *recovery_accelerated;
	double rate;
	double amount;
} RECOVERY;

/* Usage */
/* ----- */
RECOVERY *recovery_evaluate(
		char *asset_name,
		char *serial_label,
		int tax_year,
		double cost_basis,
		char *service_placement_date_string,
		char *disposal_date_string,
		char *cost_recovery_class_year_string,
		char *cost_recovery_method,
		char *cost_recovery_convention,
		char *cost_recovery_system );

/* Process */
/* ------- */
DATE *recovery_service_placement_date(
		char *service_placement_date_string );

DATE *recovery_disposal_date(
		char *disposal_date_string );

double recovery_class_year(
		char *cost_recovery_class_year_string );

double recovery_rate(
		RECOVERY_STRAIGHT_LINE *recovery_straight_line,
		RECOVERY_ACCELERATED *recovery_accelerated );

double recovery_amount(
		RECOVERY_STRAIGHT_LINE *recovery_straight_line,
		RECOVERY_ACCELERATED *recovery_accelerated );

/* Usage */
/* ----- */
RECOVERY *recovery_fetch(
		char *asset_name,
		char *serial_label,
		int tax_year );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *recovery_primary_where(
		char *asset_name,
		char *serial_label,
		int tax_year );

/* Usage */
/* ----- */
LIST *recovery_list_fetch(
		char *asset_name,
		char *serial_label );

/* Usage */
/* ----- */
LIST *recovery_system_list(
		char *appaserver_system_string );

/* Usage */
/* ----- */
RECOVERY *recovery_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RECOVERY *recovery_new(
		char *asset_name,
		char *serial_label,
		int tax_year );

/* Process */
/* ------- */
RECOVERY *recovery_calloc(
		void );

/* Usage */
/* ----- */
int recovery_year(
		int tax_year,
		int service_placement_year,
		int disposal_year );

/* Usage */
/* ----- */
void recovery_list_insert(
		char *recovery_table,
		LIST *recovery_list );

/* Process */
/* ------- */
FILE *recovery_insert_open(
		char *recovery_table );

void recovery_insert(
		FILE *recovery_insert_open,
		char *asset_name,
		char *serial_label,
		int tax_year,
		double recovery_rate,
		double recovery_amount );

/* Usage */
/* ----- */
void recovery_update(
		char *asset_name,
		char *serial_label,
		int tax_year,
		double recovery_rate,
		double recovery_amount );

/* Process */
/* ------- */
FILE *recovery_update_open(
		char *recovery_table );

/* Usage */
/* ----- */
FILE *recovery_delete_open(
		char *recovery_table );

/* Public */
/* ------ */
char *recovery_subquery_where(
		int tax_year );

void recovery_fixed_asset_list_set(
		LIST *fixed_asset_list,
		int tax_year );

void recovery_fixed_assets_list_display(
		FILE *output_pipe,
		LIST *fixed_asset_list );

int recovery_prior_tax_year(
		char *recovery_table );

#endif
