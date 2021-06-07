/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/recovery.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef RECOVERY_H
#define RECOVERY_H

#include "list.h"
#include "boolean.h"
#include "String.h"
#include "html_table.h"
#include "date.h"

/* Constants */
/* --------- */
#define COST_RECOVERY_TABLE			"cost_recovery"
#define RECOVERY_STATUTORY_STRAIGHT_LINE	"statutory_straight_line"
#define RECOVERY_STATUTORY_ACCELERATED		"statutory_accelerated"
#define RECOVERY_HALF_YEAR			"half_year"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	int tax_year;
	double cost_basis;
	int service_placement_month;
	int service_placement_year;
	int disposal_month;
	int disposal_year;
	double recovery_period_years;
	char *cost_recovery_conversion;

	/* Process */
	/* ------- */
	int service_placement_years;
	double straight_line_amount;
	double straight_line_rate;
} RECOVERY_STRAIGHT_LINE;

typedef struct
{
	/* Input */
	/* ----- */
	int tax_year;
	double cost_basis;
	int service_placement_month;
	int service_placement_year;
	int disposal_month;
	int disposal_year;
	double recovery_period_years;
	char *cost_recovery_conversion;

	/* Process */
	/* ------- */
	int service_placement_years;
	double accelerated_amount;
	double accelerated_rate;
} RECOVERY_ACCELERATED;

typedef struct
{
	/* Input */
	/* ----- */
	char *asset_name;
	char *serial_label;
	int tax_year;
	double cost_basis;
	char *service_placement_date;
	char *disposal_date;
	char *cost_recovery_period_string;
	char *cost_recovery_method;
	char *cost_recovery_conversion;

	/* Process */
	/* ------- */
	double recovery_amount;
	double recovery_rate;
	double recovery_period_years;
	int service_placement_month;
	int service_placement_year;
	int disposal_month;
	int disposal_year;
	RECOVERY_STRAIGHT_LINE *straight_line;
	RECOVERY_ACCELERATED *accelerated;
} RECOVERY;

/* Operations */
/* ---------- */
RECOVERY_STRAIGHT_LINE *recovery_straight_line_calloc(
			void );

RECOVERY_ACCELERATED *recovery_accelerated_calloc(
			void );

RECOVERY *recovery_evaluate(
			char *asset_name,
			char *serial_label,
			int tax_year,
			double cost_basis,
			char *service_placement_date,
			char *disposal_date,
			char *cost_recovery_period_string,
			char *cost_recovery_method,
			char *cost_recovery_conversion );

RECOVERY *recovery_fetch(
			char *asset_name,
			char *serial_label,
			int tax_year );

RECOVERY *recovery_new(
			char *asset_name,
			char *serial_label,
			int tax_year );

void recovery_fixed_asset_list_set(
			LIST *fixed_asset_list,
			int tax_year );

void recovery_fixed_assets_list_display(
			FILE *output_pipe,
			LIST *fixed_asset_list );

void recovery_list_insert(
			LIST *cost_recovery_list );

void recovery_fixed_asset_list_display(
			FILE *output_pipe,
			LIST *fixed_asset_list );

RECOVERY *recovery_parse(
			char *input );

LIST *recovery_list_fetch(
			char *asset_name,
			char *serial_label );

char *recovery_system_string(
			char *where );

LIST *recovery_system_list(
			char *system_string );

char *recovery_primary_where(
			char *asset_name,
			char *serial_label,
			int tax_year );

FILE *recovery_insert_open(
			void );

void recovery_insert(
			FILE *insert_pipe,
			char *asset_name,
			char *serial_label,
			int tax_year,
			double recovery_amount );

RECOVERY_STRAIGHT_LINE *recovery_statutory_straight_line_evaluate(
			int tax_year,
			double cost_basis,
			int service_placement_month,
			int service_placement_year,
			int disposal_month,
			int disposal_year,
			double recovery_period_years,
			char *cost_recovery_conversion );

RECOVERY_ACCELERATED *recovery_statutory_accelerated_evaluate(
			int tax_year,
			double cost_basis,
			int service_placement_month,
			int service_placement_year,
			int disposal_month,
			int disposal_year,
			double recovery_period_years,
			char *cost_recovery_conversion );

double recovery_period_years(
			char *cost_recovery_period_string );

int recovery_service_placement_years(
			int tax_year,
			int service_placement_year );

int recovery_prior_tax_year(
			void );

FILE *recovery_delete_open(
			void );

char *recovery_subquery_where(
			int tax_year );

double recovery_amount(
			RECOVERY_STRAIGHT_LINE *straight_line,
			RECOVERY_ACCELERATED *accelerated );

double recovery_rate(
			RECOVERY_STRAIGHT_LINE *straight_line,
			RECOVERY_ACCELERATED *accelerated );

FILE *recovery_update_open(
			void );

void recovery_update(	double recovery_amount,
			double recovery_rate,
			char *asset_name,
			char *serial_label,
			int tax_year );

double recovery_statutory_straight_line_half_year_rate(
			int service_placement_years,
			int tax_year,
			int disposal_year,
			double recovery_period_years );

double recovery_statutory_straight_line_half_year_mid_year_rate(
			double recovery_period_years );

double recovery_statutory_straight_line_half_year_book_ends_rate(
			double half_year_mid_year_rate );

double recovery_straight_line_amount(
			double straight_line_rate,
			double cost_basis );

double recovery_statutory_accelerated_half_year_rate(
			int service_placement_years,
			int tax_year,
			int disposal_year,
			double recovery_period_years );

double recovery_statutory_accelerated_half_year_evaluate(
			int service_placement_years,
			double recovery_period_years );

double recovery_accelerated_amount(
			double accelerated_rate,
			double cost_basis );

double recovery_rate(
			RECOVERY_STRAIGHT_LINE *straight_line,
			RECOVERY_ACCELERATED *accelerated );

#endif
