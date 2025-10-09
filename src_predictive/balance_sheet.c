/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/balance_sheet.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "appaserver_error.h"
#include "income_statement.h"
#include "journal.h"
#include "balance_sheet.h"

BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
	balance_sheet_subclass_display_html_new(
		LIST *element_statement_list,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
		balance_sheet_subclass_display_html;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_subclass_display_html =
		balance_sheet_subclass_display_html_calloc();

	balance_sheet_subclass_display_html->
		statement_subclass_display_html_list =
			statement_subclass_display_html_list_new(
				element_statement_list,
				statement_prior_year_list );

	if ( !balance_sheet_subclass_display_html->
		statement_subclass_display_html_list )
	{
		free( balance_sheet_subclass_display_html );
		return NULL;
	}

	balance_sheet_subclass_display_html->row_list =
		statement_subclass_display_html_list_row_list(
			balance_sheet_subclass_display_html->
				statement_subclass_display_html_list );

	list_set_list(
		balance_sheet_subclass_display_html->row_list,
			balance_sheet_subclass_display_html_equity_row_list(
				balance_sheet_equity,
				income_statement_net_income_label ) );

	return balance_sheet_subclass_display_html;
}

BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
	balance_sheet_subclass_display_html_calloc(
		void )
{
	BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
		balance_sheet_subclass_display_html;

	if ( ! ( balance_sheet_subclass_display_html =
		   calloc(
			1,
			sizeof ( BALANCE_SHEET_SUBCLASS_DISPLAY_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_subclass_display_html;
}

LIST *balance_sheet_subclass_display_html_equity_row_list(
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		char *income_statement_net_income_label )
{
	LIST *row_list;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	list_set(
		row_list,
		balance_sheet_subclass_display_html_equity_begin_row(
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	if ( balance_sheet_equity->transaction_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_html_row(
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity->transaction_amount ) );
	}

	if ( balance_sheet_equity->drawing_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_html_row(
			BALANCE_SHEET_DRAWING_LABEL,
			-balance_sheet_equity->drawing_amount ) );
	}

	list_set(
		row_list,
		balance_sheet_html_net_income_row(
			balance_sheet_equity->
				income_statement_fetch_net_income,
			income_statement_net_income_label ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_html_equity_end_row(
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity->end_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_html_liability_plus_equity_row(
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity->liability_plus_equity ) );

	return row_list;
}

HTML_ROW *balance_sheet_subclass_display_html_equity_begin_row(
		char *balance_sheet_begin_balance_label,
		double element_equity_begin_balance )
{
	LIST *cell_list;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_begin_balance_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					element_equity_begin_balance ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_equity_html_row(
		const char *label,
		double amount )
{
	LIST *cell_list;

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( (char *)label ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money( amount ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_html_net_income_row(
		double income_statement_fetch_net_income,
		char *income_statement_net_income_label )
{
	LIST *cell_list;

	if ( !income_statement_net_income_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: income_statement_net_income_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				income_statement_net_income_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
					income_statement_fetch_net_income ) ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_subclass_display_html_equity_end_row(
		char *balance_sheet_end_balance_label,
		double balance_sheet_equity_end_balance )
{
	LIST *cell_list;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_end_balance_label ),
			1 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					balance_sheet_equity_end_balance ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_subclass_display_html_liability_plus_equity_row(
		char *balance_sheet_liability_plus_equity_label,
		double balance_sheet_liability_plus_equity )
{
	LIST *cell_list;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_liability_plus_equity_label ),
			1 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
				    balance_sheet_liability_plus_equity ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

BALANCE_SHEET_SUBCLASS_OMIT_HTML *
	balance_sheet_subclass_omit_html_new(
		LIST *element_statement_list,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_OMIT_HTML *
		balance_sheet_subclass_omit_html;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_subclass_omit_html =
		balance_sheet_subclass_omit_html_calloc();

	balance_sheet_subclass_omit_html->
		statement_subclass_omit_html_list =
			statement_subclass_omit_html_list_new(
				element_statement_list,
				statement_prior_year_list );

	if ( !balance_sheet_subclass_omit_html->
		statement_subclass_omit_html_list )
	{
		free( balance_sheet_subclass_omit_html );
		return NULL;
	}

	balance_sheet_subclass_omit_html->row_list =
		statement_subclass_omit_html_list_row_list(
			balance_sheet_subclass_omit_html->
				statement_subclass_omit_html_list );

	list_set_list(
		balance_sheet_subclass_omit_html->row_list,
			balance_sheet_subclass_omit_html_equity_row_list(
				balance_sheet_equity,
				income_statement_net_income_label ) );

	return balance_sheet_subclass_omit_html;
}

BALANCE_SHEET_SUBCLASS_OMIT_HTML *
	balance_sheet_subclass_omit_html_calloc(
		void )
{
	BALANCE_SHEET_SUBCLASS_OMIT_HTML *
		balance_sheet_subclass_omit_html;

	if ( ! ( balance_sheet_subclass_omit_html =
		    calloc(
			1, 
			sizeof ( BALANCE_SHEET_SUBCLASS_OMIT_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_subclass_omit_html;
}

LIST *balance_sheet_subclass_omit_html_equity_row_list(
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		char *income_statement_net_income_label )
{
	LIST *row_list;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	list_set(
		row_list,
		balance_sheet_subclass_omit_html_equity_begin_row(
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	if ( balance_sheet_equity->transaction_amount )
	{
		list_set(
			row_list,
			balance_sheet_equity_html_row(
				BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
				balance_sheet_equity->transaction_amount ) );
	}

	if ( balance_sheet_equity->drawing_amount )
	{
		list_set(
			row_list,
			balance_sheet_equity_html_row(
				BALANCE_SHEET_DRAWING_LABEL,
				-balance_sheet_equity->drawing_amount ) );
	}

	list_set(
		row_list,
		balance_sheet_html_net_income_row(
			balance_sheet_equity->income_statement_fetch_net_income,
			income_statement_net_income_label ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_html_equity_end_row(
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity->end_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_html_liability_plus_equity_row(
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity->liability_plus_equity ) );

	return row_list;
}

HTML_ROW *balance_sheet_subclass_omit_html_equity_begin_row(
		char *balance_sheet_begin_balance_label,
		double element_equity_begin_balance )
{
	LIST *cell_list;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_begin_balance_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					element_equity_begin_balance ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_subclass_omit_html_equity_end_row(
		char *balance_sheet_end_balance_label,
		double balance_sheet_equity_end_balance )
{
	LIST *cell_list;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_end_balance_label ),
			1 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					balance_sheet_equity_end_balance ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_subclass_omit_html_liability_plus_equity_row(
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity )
{
	LIST *cell_list;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_liability_plus_equity_label ),
			1 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
				    balance_sheet_liability_plus_equity ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

BALANCE_SHEET_SUBCLASS_AGGR_HTML *
	balance_sheet_subclass_aggr_html_new(
		LIST *element_statement_list,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_AGGR_HTML *
		balance_sheet_subclass_aggr_html;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_subclass_aggr_html =
		balance_sheet_subclass_aggr_html_calloc();

	balance_sheet_subclass_aggr_html->
		statement_subclass_aggr_html_list =
			statement_subclass_aggr_html_list_new(
				element_statement_list,
				statement_prior_year_list );

	if ( !balance_sheet_subclass_aggr_html->
		statement_subclass_aggr_html_list )
	{
		free( balance_sheet_subclass_aggr_html );
		return NULL;
	}

	balance_sheet_subclass_aggr_html->row_list =
		statement_subclass_aggr_html_list_row_list(
			balance_sheet_subclass_aggr_html->
				statement_subclass_aggr_html_list );

	list_set_list(
		balance_sheet_subclass_aggr_html->row_list,
		balance_sheet_subclass_aggr_html_equity_row_list(
			balance_sheet_equity,
			income_statement_net_income_label ) );

	return balance_sheet_subclass_aggr_html;
}

BALANCE_SHEET_SUBCLASS_AGGR_HTML *
	balance_sheet_subclass_aggr_html_calloc(
			void )
{
	BALANCE_SHEET_SUBCLASS_AGGR_HTML *
		balance_sheet_subclass_aggr_html;

	if ( ! ( balance_sheet_subclass_aggr_html =
		   calloc( 1,
			   sizeof ( BALANCE_SHEET_SUBCLASS_AGGR_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_subclass_aggr_html;
}

LIST *balance_sheet_subclass_aggr_html_equity_row_list(
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		char *income_statement_net_income_label )
{
	LIST *row_list;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	list_set(
		row_list,
		balance_sheet_subclass_aggr_html_equity_begin_row(
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	if ( balance_sheet_equity->transaction_amount )
	{
		list_set(
			row_list,
			balance_sheet_equity_html_row(
				BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
				balance_sheet_equity->transaction_amount ) );
	}

	if ( balance_sheet_equity->drawing_amount )
	{
		list_set(
			row_list,
			balance_sheet_equity_html_row(
				BALANCE_SHEET_DRAWING_LABEL,
				-balance_sheet_equity->drawing_amount ) );
	}

	list_set(
		row_list,
		balance_sheet_html_net_income_row(
			balance_sheet_equity->income_statement_fetch_net_income,
			income_statement_net_income_label ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_html_equity_end_row(
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity->end_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_html_liability_plus_equity_row(
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity->liability_plus_equity ) );

	return row_list;
}

HTML_ROW *balance_sheet_subclass_aggr_html_equity_begin_row(
		char *balance_sheet_begin_balance_label,
		double element_equity_begin_balance )
{
	LIST *cell_list;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_begin_balance_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					element_equity_begin_balance ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_subclass_aggr_html_equity_end_row(
		char *balance_sheet_end_balance_label,
		double balance_sheet_equity_end_balance )
{
	LIST *cell_list;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_end_balance_label ),
			1 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					balance_sheet_equity_end_balance ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

HTML_ROW *balance_sheet_subclass_aggr_html_liability_plus_equity_row(
		char *balance_sheet_liability_plus_equity_label,
		double balance_sheet_liability_plus_equity )
{
	LIST *cell_list;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_liability_plus_equity_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
					balance_sheet_liability_plus_equity ) ),
			0 /* not large_boolean */,
			0 /* bold_boolean */ ) );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

BALANCE_SHEET_HTML *balance_sheet_html_new(
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label )
{
	BALANCE_SHEET_HTML *balance_sheet_html;

	if ( !statement
	||   !statement->statement_caption
	||   !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_html = balance_sheet_html_calloc();

	if (	statement_subclassification_option ==
		statement_subclassification_display )
	{
		balance_sheet_html->
			balance_sheet_subclass_display_html =
				balance_sheet_subclass_display_html_new(
					statement->element_statement_list,
					balance_sheet_equity,
					statement_prior_year_list,
					income_statement_net_income_label );

		if ( !balance_sheet_html->balance_sheet_subclass_display_html )
		{
			free( balance_sheet_html );
			return NULL;
		}

		balance_sheet_html->html_table =
			balance_sheet_html_table(
				statement->statement_caption->sub_title,
				balance_sheet_html->
					balance_sheet_subclass_display_html->
					statement_subclass_display_html_list->
					column_list,
				balance_sheet_html->
					balance_sheet_subclass_display_html->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_omit )
	{
		balance_sheet_html->
			balance_sheet_subclass_omit_html =
				balance_sheet_subclass_omit_html_new(
					statement->element_statement_list,
					balance_sheet_equity,
					statement_prior_year_list,
					income_statement_net_income_label );

		if ( ! balance_sheet_html->
			balance_sheet_subclass_omit_html )
		{
			free( balance_sheet_html );
			return NULL;
		}

		balance_sheet_html->html_table =
			balance_sheet_html_table(
				statement->statement_caption->sub_title,
				balance_sheet_html->
					balance_sheet_subclass_omit_html->
					statement_subclass_omit_html_list->
					column_list,
				balance_sheet_html->
					balance_sheet_subclass_omit_html->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_aggregate )
	{
		balance_sheet_html->
			balance_sheet_subclass_aggr_html =
				balance_sheet_subclass_aggr_html_new(
					statement->element_statement_list,
					balance_sheet_equity,
					statement_prior_year_list,
					income_statement_net_income_label );

		if ( ! balance_sheet_html->
			balance_sheet_subclass_aggr_html )
		{
			free( balance_sheet_html );
			return NULL;
		}

		balance_sheet_html->html_table =
			balance_sheet_html_table(
				statement->statement_caption->sub_title,
				balance_sheet_html->
					balance_sheet_subclass_aggr_html->
					statement_subclass_aggr_html_list->
					column_list,
				balance_sheet_html->
					balance_sheet_subclass_aggr_html->
					row_list );
	}

	return balance_sheet_html;
}

BALANCE_SHEET_HTML *balance_sheet_html_calloc( void )
{
	BALANCE_SHEET_HTML *balance_sheet_html;

	if ( ! ( balance_sheet_html =
			calloc( 1, sizeof ( BALANCE_SHEET_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_html;
}

HTML_TABLE *balance_sheet_html_table(
		char *statement_caption_sub_title,
		LIST *column_list,
		LIST *row_list )
{
	HTML_TABLE *html_table;

	if ( !statement_caption_sub_title
	||   !list_length( column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	html_table =
		html_table_new(
			(char *)0 /* title */,
			statement_caption_sub_title,
			(char *)0 /* sub_sub_title */ );

	html_table->column_list = column_list;
	html_table->row_list = row_list;

	return html_table;
}

BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
	balance_sheet_subclass_display_latex_new(
		LIST *element_statement_list,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
		balance_sheet_subclass_display_latex;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_subclass_display_latex =
		balance_sheet_subclass_display_latex_calloc();

	balance_sheet_subclass_display_latex->
		statement_subclass_display_latex_list =
			statement_subclass_display_latex_list_new(
				element_statement_list,
				statement_prior_year_list );

	if ( !balance_sheet_subclass_display_latex->
		statement_subclass_display_latex_list )
	{
		free( balance_sheet_subclass_display_latex );
		return NULL;
	}

	balance_sheet_subclass_display_latex->row_list =
		statement_subclass_display_latex_list_row_list(
			balance_sheet_subclass_display_latex->
				statement_subclass_display_latex_list );

	list_set_list(
		balance_sheet_subclass_display_latex->row_list,
		balance_sheet_subclass_display_latex_equity_row_list(
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */,
			balance_sheet_equity->begin_balance,
			balance_sheet_equity->transaction_amount,
			balance_sheet_equity->drawing_amount,
			balance_sheet_equity->end_balance,
			balance_sheet_equity->liability_plus_equity,
			balance_sheet_equity->
				income_statement_fetch_net_income,
			income_statement_net_income_label,
			balance_sheet_subclass_display_latex->
				statement_subclass_display_latex_list->
				column_list
				/* latex_column_list */ ) );

	return balance_sheet_subclass_display_latex;
}

BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
	balance_sheet_subclass_display_latex_calloc(
		void )
{
	BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
		balance_sheet_subclass_display_latex;

	if ( ! ( balance_sheet_subclass_display_latex =
		   calloc(
			1,
			sizeof ( BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_subclass_display_latex;
}

LIST *balance_sheet_subclass_display_latex_equity_row_list(
		int statement_prior_year_list_length,
		double balance_sheet_equity_begin_balance,
		double balance_sheet_equity_transaction_amount,
		double balance_sheet_equity_drawing_amount,
		double balance_sheet_equity_end_balance,
		double balance_sheet_equity_liability_plus_equity,
		double income_statement_fetch_net_income,
		char *income_statement_net_income_label,
		LIST *latex_column_list )
{
	LIST *row_list;

	if ( !income_statement_net_income_label
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_equity_begin_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity_begin_balance,
			latex_column_list ) );

	if ( balance_sheet_equity_transaction_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_latex_row(
			statement_prior_year_list_length + 3
				/* empty_cell_count */,
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity_transaction_amount,
			latex_column_list ) );
	}

	if ( balance_sheet_equity_drawing_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_latex_row(
			statement_prior_year_list_length + 3
				/* empty_cell_count */,
			BALANCE_SHEET_DRAWING_LABEL,
			-balance_sheet_equity_drawing_amount,
			latex_column_list ) );
	}

	list_set(
		row_list,
		balance_sheet_latex_net_income_row(
			income_statement_fetch_net_income,
			income_statement_net_income_label,
			statement_prior_year_list_length + 3
				/* empty_cell_count */,
			latex_column_list ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_equity_end_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity_end_balance,
			latex_column_list ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_liability_plus_equity_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity_liability_plus_equity,
			latex_column_list ) );

	return row_list;
}

LATEX_ROW *balance_sheet_subclass_display_latex_equity_begin_row(
		int statement_prior_year_list_length,
		char *balance_sheet_begin_balance_label,
		double element_equity_begin_balance,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_begin_balance_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_begin_balance_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
					element_equity_begin_balance ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 3;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

LATEX_ROW *balance_sheet_equity_latex_row(
		int empty_cell_count,
		const char *label,
		double amount,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "latex_column_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( (char *)label ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
			    /* --------------------- */
			    /* Returns static memory */
			    /* --------------------- */
			    string_commas_money( amount ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < empty_cell_count;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) ),

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

LATEX_ROW *balance_sheet_subclass_display_latex_equity_end_row(
		int statement_prior_year_list_length,
		char *balance_sheet_end_balance_label,
		double balance_sheet_equity_end_balance,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_end_balance_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_end_balance_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 2;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
			    /* --------------------- */
			    /* Returns static memory */
			    /* --------------------- */
			    string_commas_money(
				    balance_sheet_equity_end_balance ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 1;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

LATEX_ROW *balance_sheet_subclass_display_latex_liability_plus_equity_row(
		int statement_prior_year_list_length,
		char *balance_sheet_liability_plus_equity_label,
		double balance_sheet_liability_plus_equity,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_liability_plus_equity_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_liability_plus_equity_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < 2;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
				    balance_sheet_liability_plus_equity ) ) ) );

	for (	i = 0;
		i < statement_prior_year_list_length + 1;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
	balance_sheet_subclass_omit_latex_new(
		LIST *element_statement_list,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
		balance_sheet_subclass_omit_latex;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_subclass_omit_latex =
		balance_sheet_subclass_omit_latex_calloc();

	balance_sheet_subclass_omit_latex->
		statement_subclass_omit_latex_list =
			statement_subclass_omit_latex_list_new(
				element_statement_list,
				statement_prior_year_list );

	if ( !balance_sheet_subclass_omit_latex->
		statement_subclass_omit_latex_list )
	{
		free( balance_sheet_subclass_omit_latex );
		return NULL;
	}

	balance_sheet_subclass_omit_latex->row_list =
		statement_subclass_omit_latex_list_row_list(
			balance_sheet_subclass_omit_latex->
				statement_subclass_omit_latex_list );

	list_set_list(
		balance_sheet_subclass_omit_latex->row_list,
			balance_sheet_subclass_omit_latex_equity_row_list(
				list_length( statement_prior_year_list )
					/* statement_prior_year_list_length */,
				balance_sheet_equity->begin_balance,
				balance_sheet_equity->transaction_amount,
				balance_sheet_equity->drawing_amount,
				balance_sheet_equity->end_balance,
				balance_sheet_equity->liability_plus_equity,
				balance_sheet_equity->
					income_statement_fetch_net_income,
				income_statement_net_income_label,
				balance_sheet_subclass_omit_latex->
					statement_subclass_omit_latex_list->
					column_list
					/* latex_column_list */ ) );

	return balance_sheet_subclass_omit_latex;
}

BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
	balance_sheet_subclass_omit_latex_calloc(
		void )
{
	BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
		balance_sheet_subclass_omit_latex;

	if ( ! ( balance_sheet_subclass_omit_latex =
		   calloc(
			1,
			sizeof ( BALANCE_SHEET_SUBCLASS_OMIT_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_subclass_omit_latex;
}

LIST *balance_sheet_subclass_omit_latex_equity_row_list(
		int statement_prior_year_list_length,
		double balance_sheet_equity_begin_balance,
		double balance_sheet_equity_transaction_amount,
		double balance_sheet_equity_drawing_amount,
		double balance_sheet_equity_end_balance,
		double balance_sheet_equity_liability_plus_equity,
		double income_statement_fetch_net_income,
		char *income_statement_net_income_label,
		LIST *latex_column_list )
{
	LIST *row_list;

	if ( !income_statement_net_income_label
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_equity_begin_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity_begin_balance,
			latex_column_list ) );

	if ( balance_sheet_equity_transaction_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_latex_row(
			statement_prior_year_list_length + 2
				/* empty_cell_count */,
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity_transaction_amount,
			latex_column_list ) );
	}

	if ( balance_sheet_equity_drawing_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_latex_row(
			statement_prior_year_list_length + 2
				/* empty_cell_count */,
			BALANCE_SHEET_DRAWING_LABEL,
			-balance_sheet_equity_drawing_amount,
			latex_column_list ) );
	}

	list_set(
		row_list,
		balance_sheet_latex_net_income_row(
			income_statement_fetch_net_income,
			income_statement_net_income_label,
			statement_prior_year_list_length + 2
				/* empty_cell_count */,
			latex_column_list ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_equity_end_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity_end_balance,
			latex_column_list ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_liability_plus_equity_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity_liability_plus_equity,
			latex_column_list ) );

	return row_list;
}

LATEX_ROW *balance_sheet_subclass_omit_latex_equity_begin_row(
		int statement_prior_year_list_length,
		char *balance_sheet_begin_balance_label,
		double element_equity_begin_balance,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_begin_balance_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_begin_balance_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
					element_equity_begin_balance ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 2;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

LATEX_ROW *balance_sheet_subclass_omit_latex_equity_end_row(
		int statement_prior_year_list_length,
		char *balance_sheet_end_balance_label,
		double balance_sheet_equity_end_balance,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_end_balance_label
	||   !list_rewind( latex_column_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_end_balance_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
				    balance_sheet_equity_end_balance ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 1;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );

	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

LATEX_ROW *balance_sheet_subclass_omit_latex_liability_plus_equity_row(
		int statement_prior_year_list_length,
		char *balance_sheet_liability_plus_equity_label,
		double balance_sheet_liability_plus_equity,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_liability_plus_equity_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_liability_plus_equity_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
				    balance_sheet_liability_plus_equity ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 1;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );

	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_new(
		LIST *element_statement_list,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
		balance_sheet_subclass_aggr_latex;

	if ( !balance_sheet_equity
	||   !income_statement_net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_subclass_aggr_latex =
		balance_sheet_subclass_aggr_latex_calloc();

	balance_sheet_subclass_aggr_latex->
		statement_subclass_aggr_latex_list =
			statement_subclass_aggr_latex_list_new(
				element_statement_list,
				statement_prior_year_list );

	if ( !balance_sheet_subclass_aggr_latex->
		statement_subclass_aggr_latex_list )
	{
		free( balance_sheet_subclass_aggr_latex );
		return NULL;
	}

	balance_sheet_subclass_aggr_latex->row_list =
		statement_subclass_aggr_latex_list_row_list(
			balance_sheet_subclass_aggr_latex->
				statement_subclass_aggr_latex_list );

	list_set_list(
		balance_sheet_subclass_aggr_latex->row_list,
			balance_sheet_subclass_aggr_latex_equity_row_list(
				list_length( statement_prior_year_list )
					/* statement_prior_year_list_length */,
				balance_sheet_equity->begin_balance,
				balance_sheet_equity->transaction_amount,
				balance_sheet_equity->drawing_amount,
				balance_sheet_equity->end_balance,
				balance_sheet_equity->liability_plus_equity,
				balance_sheet_equity->
					income_statement_fetch_net_income,
				income_statement_net_income_label,
				balance_sheet_subclass_aggr_latex->
					statement_subclass_aggr_latex_list->
					column_list
					/* latex_column_list */ ) );

	return balance_sheet_subclass_aggr_latex;
}

BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_calloc(
		void )
{
	BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
		balance_sheet_subclass_aggr_latex;

	if ( ! ( balance_sheet_subclass_aggr_latex =
		    calloc(
			1,
			sizeof ( BALANCE_SHEET_SUBCLASS_AGGR_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_subclass_aggr_latex;
}

LIST *balance_sheet_subclass_aggr_latex_equity_row_list(
		int statement_prior_year_list_length,
		double balance_sheet_equity_begin_balance,
		double balance_sheet_equity_transaction_amount,
		double balance_sheet_equity_drawing_amount,
		double balance_sheet_equity_end_balance,
		double balance_sheet_equity_liability_plus_equity,
		double income_statement_fetch_net_income,
		char *income_statement_net_income_label,
		LIST *latex_column_list )
{
	LIST *row_list;

	if ( !income_statement_net_income_label
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_list = list_new();

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_equity_begin_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity_begin_balance,
			latex_column_list ) );

	if ( balance_sheet_equity_transaction_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_latex_row(
			statement_prior_year_list_length + 2
				/* empty_cell_count */,
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity_transaction_amount,
			latex_column_list ) );
	}

	if ( balance_sheet_equity_drawing_amount )
	{
		list_set(
		    row_list,
		    balance_sheet_equity_latex_row(
			statement_prior_year_list_length + 2
				/* empty_cell_count */,
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			-balance_sheet_equity_drawing_amount,
			latex_column_list ) );
	}

	list_set(
		row_list,
		balance_sheet_latex_net_income_row(
			income_statement_fetch_net_income,
			income_statement_net_income_label,
			statement_prior_year_list_length + 2
				/* empty_cell_count */,
			latex_column_list ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_equity_end_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity_end_balance,
			latex_column_list ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_liability_plus_equity_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity_liability_plus_equity,
			latex_column_list ) );

	return row_list;
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_equity_begin_row(
		int statement_prior_year_list_length,
		char *balance_sheet_begin_balance_label,
		double element_equity_begin_balance,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_begin_balance_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_begin_balance_label ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
					element_equity_begin_balance ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 2;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_equity_end_row(
		int statement_prior_year_list_length,
		char *balance_sheet_end_balance_label,
		double balance_sheet_equity_end_balance,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_end_balance_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_end_balance_label ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
			    /* --------------------- */
			    /* Returns static memory */
			    /* --------------------- */
			    string_commas_money(
				balance_sheet_equity_end_balance ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 1;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_liability_plus_equity_row(
		int statement_prior_year_list_length,
		char *balance_sheet_liability_plus_equity_label,
		double balance_sheet_liability_plus_equity,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !balance_sheet_liability_plus_equity_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				balance_sheet_liability_plus_equity_label ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
			    /* --------------------- */
			    /* Returns static memory */
			    /* --------------------- */
			    string_commas_money(
				balance_sheet_liability_plus_equity ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 1;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

BALANCE_SHEET_LATEX *balance_sheet_latex_new(
		char *application_name,
		char *process_name,
		char *data_directory,
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		BALANCE_SHEET_EQUITY *balance_sheet_equity,
		LIST *statement_prior_year_list,
		char *income_statement_net_income_label,
		pid_t process_id )
{
	BALANCE_SHEET_LATEX *balance_sheet_latex;

	if ( !application_name
	||   !process_name
	||   !data_directory
	||   !statement
	||   !statement->statement_caption
	||   !balance_sheet_equity
	||   !income_statement_net_income_label
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet_latex = balance_sheet_latex_calloc();

	balance_sheet_latex->statement_link =
		statement_link_new(
			application_name,
			process_name,
			data_directory,
			statement->transaction_date_begin_date_string,
			statement->end_date_time,
			process_id );

	balance_sheet_latex->latex =
		latex_new(
			balance_sheet_latex->
				statement_link->
				tex_filename,
			balance_sheet_latex->
				statement_link->
				appaserver_link_working_directory,
			statement->pdf_landscape_boolean,
			statement->
				statement_caption->
				logo_filename );

	if (	statement_subclassification_option ==
		statement_subclassification_display )
	{
		balance_sheet_latex->balance_sheet_subclass_display_latex =
			balance_sheet_subclass_display_latex_new(
				statement->element_statement_list,
				balance_sheet_equity,
				statement_prior_year_list,
				income_statement_net_income_label );

		if ( !balance_sheet_latex->
			balance_sheet_subclass_display_latex )
		{
			free( balance_sheet_latex );
			return NULL;
		}

		balance_sheet_latex->latex_table =
			balance_sheet_latex_table(
				statement->statement_caption->combined,
				balance_sheet_latex->
					balance_sheet_subclass_display_latex->
					statement_subclass_display_latex_list->
					column_list,
				balance_sheet_latex->
					balance_sheet_subclass_display_latex->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_omit )
	{
		balance_sheet_latex->balance_sheet_subclass_omit_latex =
			balance_sheet_subclass_omit_latex_new(
				statement->element_statement_list,
				balance_sheet_equity,
				statement_prior_year_list,
				income_statement_net_income_label );

		if ( !balance_sheet_latex->
			balance_sheet_subclass_omit_latex )
		{
			free( balance_sheet_latex );
			return NULL;
		}

		balance_sheet_latex->latex_table =
			balance_sheet_latex_table(
				statement->statement_caption->combined,
				balance_sheet_latex->
					balance_sheet_subclass_omit_latex->
					statement_subclass_omit_latex_list->
					column_list,
				balance_sheet_latex->
					balance_sheet_subclass_omit_latex->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_aggregate )
	{
		balance_sheet_latex->balance_sheet_subclass_aggr_latex =
			balance_sheet_subclass_aggr_latex_new(
				statement->element_statement_list,
				balance_sheet_equity,
				statement_prior_year_list,
				income_statement_net_income_label );

		if ( !balance_sheet_latex->
			balance_sheet_subclass_aggr_latex )
		{
			free( balance_sheet_latex );
			return NULL;
		}

		balance_sheet_latex->latex_table =
			balance_sheet_latex_table(
				statement->statement_caption->combined,
				balance_sheet_latex->
					balance_sheet_subclass_aggr_latex->
					statement_subclass_aggr_latex_list->
					column_list,
				balance_sheet_latex->
					balance_sheet_subclass_aggr_latex->
					row_list );
	}

	return balance_sheet_latex;
}

BALANCE_SHEET_LATEX *balance_sheet_latex_calloc( void )
{
	BALANCE_SHEET_LATEX *balance_sheet_latex;

	if ( ! ( balance_sheet_latex =
			calloc( 1, sizeof ( BALANCE_SHEET_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_latex;
}

LATEX_TABLE *balance_sheet_latex_table(
		char *statement_caption_combined,
		LIST *column_list,
		LIST *row_list )
{
	if ( !statement_caption_combined
	||   !list_length( column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_table_new(
		statement_caption_combined /* title */,
		column_list,
		row_list );
}

BALANCE_SHEET *balance_sheet_fetch(
		char *argv_0,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *data_directory,
		char *as_of_date_string,
		int prior_year_count,
		char *subclassification_option_string,
		char *output_medium_string )
{
	BALANCE_SHEET *balance_sheet;

	if ( !argv_0
	||   !application_name
	||   !session_key
	||   !role_name
	||   !process_name
	||   !data_directory
	||   !as_of_date_string
	||   !subclassification_option_string
	||   !output_medium_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet = balance_sheet_calloc();

	balance_sheet->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassification_option_string );

	balance_sheet->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	balance_sheet->transaction_date_statement =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		transaction_date_statement_new(
			as_of_date_string );

	if ( !balance_sheet->
		transaction_date_statement->
			transaction_date_as_of )
	{
		free( balance_sheet );
		return NULL;
	}

	balance_sheet->element_name_list =
		balance_sheet_element_name_list(
			ELEMENT_ASSET,
			ELEMENT_LIABILITY );

	balance_sheet->statement =
		statement_fetch(
			application_name,
			process_name,
			prior_year_count,
			balance_sheet->element_name_list,
			balance_sheet->
				transaction_date_statement->
				transaction_date_begin_date_string,
			balance_sheet->
				transaction_date_statement->
				end_date_time,
			0 /* not fetch_transaction */,
			0 /* not latest_zero_balance_boolean */ );

	if ( !balance_sheet->statement )
	{
		char message[ 128 ];

		sprintf(message,
			"statement_fetch%s() returned empty.",
			process_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet->element_equity_current =
		element_statement_fetch(
			ELEMENT_EQUITY,
			balance_sheet->
				transaction_date_statement->
				end_date_time,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */,
			0 /* not latest_zero_balance_boolean */ );

	if ( !balance_sheet->element_equity_current )
	{
		char message[ 128 ];

		sprintf(message,
			"element_statement_fetch(%s) returned empty.",
			balance_sheet->
				transaction_date_statement->
				end_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	balance_sheet->element_equity_current->sum =
		element_sum(
			balance_sheet->element_equity_current );

	balance_sheet->element_equity_begin =
		element_statement_fetch(
			ELEMENT_EQUITY,
			balance_sheet->
				transaction_date_statement->
				prior_end_date_time,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */,
			0 /* not latest_zero_balance_boolean */ );

	if ( balance_sheet->element_equity_begin )
	{
		balance_sheet->element_equity_begin->sum =
			element_sum(
				balance_sheet->element_equity_begin );
	}

	balance_sheet->element_liability =
		element_seek(
			ELEMENT_LIABILITY,
			balance_sheet->
				statement->
				element_statement_list );

	balance_sheet->income_statement_fetch_net_income =
		income_statement_fetch_net_income(
			balance_sheet->
				transaction_date_statement->
				end_date_time );

	balance_sheet->drawing_amount =
		balance_sheet_drawing_amount(
			JOURNAL_TABLE,
			ACCOUNT_DRAWING_KEY,
			balance_sheet->
				transaction_date_statement->
				end_date_time );

	balance_sheet->balance_sheet_equity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		balance_sheet_equity_new(
			balance_sheet->element_equity_begin,
			balance_sheet->element_equity_current,
			balance_sheet->element_liability,
			balance_sheet->income_statement_fetch_net_income,
			balance_sheet->drawing_amount );

	if ( prior_year_count )
	{
		balance_sheet->statement_prior_year_list =
			statement_prior_year_list(
				balance_sheet->element_name_list,
				balance_sheet->
					transaction_date_statement->
					end_date_time,
				prior_year_count,
				balance_sheet->statement );
	}

	balance_sheet->income_statement_net_income_label =
		income_statement_net_income_label(
			argv_0 );

	if (	balance_sheet->statement_subclassification_option ==
		statement_subclassification_omit )
	{
		element_list_account_statement_list_set(
			balance_sheet->statement->element_statement_list );
	}

	if ( balance_sheet->statement_output_medium ==
		statement_output_PDF )
	{
		balance_sheet->balance_sheet_latex =
			balance_sheet_latex_new(
				application_name,
				process_name,
				data_directory,
				balance_sheet->
					statement_subclassification_option,
				balance_sheet->statement,
				balance_sheet->balance_sheet_equity,
				balance_sheet->statement_prior_year_list,
				balance_sheet->
					income_statement_net_income_label,
				getpid() /* process_id */ );
	}
	else
	if ( balance_sheet->statement_output_medium ==
		statement_output_table )
	{
		element_account_transaction_count_set(
			balance_sheet->statement->element_statement_list,
			balance_sheet->
				transaction_date_statement->
				transaction_date_begin_date_string,
			balance_sheet->
				transaction_date_statement->
				end_date_time );

		element_account_action_string_set(
			balance_sheet->statement->element_statement_list,
			application_name,
			session_key,
			login_name,
			role_name,
			balance_sheet->
				transaction_date_statement->
				transaction_date_begin_date_string,
			balance_sheet->
				transaction_date_statement->
				end_date_time );

		balance_sheet->balance_sheet_html =
			balance_sheet_html_new(
				balance_sheet->
					statement_subclassification_option,
				balance_sheet->statement,
				balance_sheet->balance_sheet_equity,
				balance_sheet->statement_prior_year_list,
				balance_sheet->
					income_statement_net_income_label );
	}

	return balance_sheet;
}

BALANCE_SHEET *balance_sheet_calloc( void )
{
	BALANCE_SHEET *balance_sheet;

	if ( ! ( balance_sheet = calloc( 1, sizeof ( BALANCE_SHEET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet;
}

LIST *balance_sheet_element_name_list(
		char *element_asset,
		char *element_liability )
{
	LIST *element_name_list = list_new();

	list_set( element_name_list, element_asset );
	list_set( element_name_list, element_liability );

	return element_name_list;
}

BALANCE_SHEET_EQUITY *balance_sheet_equity_calloc( void )
{
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( ! ( balance_sheet_equity =
			calloc( 1, sizeof ( BALANCE_SHEET_EQUITY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_equity;
}

BALANCE_SHEET_EQUITY *
	balance_sheet_equity_new(
		ELEMENT *element_equity_begin,
		ELEMENT *element_equity_current,
		ELEMENT *element_liability,
		double net_income,
		double drawing_amount )
{
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	balance_sheet_equity = balance_sheet_equity_calloc();

	balance_sheet_equity->income_statement_fetch_net_income =
		net_income;

	balance_sheet_equity->drawing_amount = drawing_amount;

	if ( element_equity_begin )
	{
		balance_sheet_equity->begin_balance =
			balance_sheet_equity_begin_balance(
				element_equity_begin );
	}

	balance_sheet_equity->current_balance =
		balance_sheet_equity_current_balance(
			element_equity_current );

	balance_sheet_equity->transaction_amount =
		balance_sheet_equity_transaction_amount(
			balance_sheet_equity->drawing_amount,
			balance_sheet_equity->current_balance,
			balance_sheet_equity->begin_balance );

	balance_sheet_equity->end_balance =
		balance_sheet_equity_end_balance(
			balance_sheet_equity->current_balance,
			net_income );

	if ( element_liability )
	{
		balance_sheet_equity->liability_balance =
			balance_sheet_equity_liability_balance(
				element_liability );
	}

	balance_sheet_equity->liability_plus_equity =
		balance_sheet_equity_liability_plus_equity(
			balance_sheet_equity->liability_balance,
			balance_sheet_equity->end_balance );

	return balance_sheet_equity;
}

double balance_sheet_equity_begin_balance(
		ELEMENT *element_equity_begin )
{
	if ( element_equity_begin )
		return element_equity_begin->sum;
	else
		return 0.0;
}

double balance_sheet_equity_current_balance(
		ELEMENT *element_equity_current )
{
	if ( element_equity_current )
		return element_equity_current->sum;
	else
		return 0.0;
}

double balance_sheet_equity_transaction_amount(
		double drawing_amount,
		double equity_current_balance,
		double equity_begin_balance )
{
	return
	equity_current_balance -
	equity_begin_balance +
	drawing_amount;
}

double balance_sheet_equity_end_balance(
		double equity_current_balance,
		double income_statement_fetch_net_income )
{
	return
	equity_current_balance +
	income_statement_fetch_net_income;
}

double balance_sheet_equity_liability_balance(
		ELEMENT *element_liability )
{
	if ( element_liability )
		return element_liability->sum;
	else
		return 0.0;
}

double balance_sheet_equity_liability_plus_equity(
		double liability_balance,
		double equity_end_balance )
{
	return
	liability_balance +
	equity_end_balance;
}

LATEX_ROW *balance_sheet_latex_net_income_row(
		double income_statement_fetch_net_income,
		char *income_statement_net_income_label,
		int empty_cell_count,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	char *cell_label_datum;
	LATEX_CELL *latex_cell;
	char *commas_money;
	int i;

	if ( !income_statement_net_income_label
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			income_statement_net_income_label );

	latex_cell =
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			cell_label_datum );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	commas_money =
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_money(
			income_statement_fetch_net_income );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup( commas_money ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < empty_cell_count;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_row_new( cell_list );
}

double balance_sheet_drawing_amount(
		const char *journal_table,
		const char *account_drawing_key,
		char *transaction_end_date_time )
{
	char *drawing_string;
	ACCOUNT_JOURNAL *account_journal;
	double drawing_amount = {0};

	if ( !transaction_end_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"transaction_end_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drawing_string =
		account_drawing_string(
			account_drawing_key );

	if ( !drawing_string ) return 0.0;

	account_journal =
		account_journal_latest(
			journal_table,
			drawing_string /* account_name */,
			transaction_end_date_time,
			0 /* not fetch_transaction */,
			0 /* not latest_zero_balance_boolean */ );

	if ( account_journal )
		drawing_amount =
			account_journal->balance;

	return drawing_amount;
}

