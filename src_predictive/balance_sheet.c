/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/balance_sheet.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "timlib.h"
#include "transaction.h"
#include "statement.h"
#include "element.h"
#include "latex.h"
#include "html_table.h"
#include "income_statement.h"
#include "balance_sheet.h"

BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
	balance_sheet_subclass_display_html_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
		balance_sheet_subclass_display_html;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
		return (BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *)0;
	}

	balance_sheet_subclass_display_html->row_list =
		statement_subclass_display_html_list_extract_row_list(
			balance_sheet_subclass_display_html->
				statement_subclass_display_html_list );

	list_set_list(
		balance_sheet_subclass_display_html->row_list,
			balance_sheet_subclass_display_html_equity_row_list(
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
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
			sizeof( BALANCE_SHEET_SUBCLASS_DISPLAY_HTML ) ) ) )
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
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LIST *row_list;
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_list = list_new();

	balance_sheet_equity =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		balance_sheet_equity_new(
			element_equity_begin,
			element_equity_current,
			element_liability,
			income_statement_fetch_net_income );

	list_set(
		row_list,
		balance_sheet_subclass_display_html_equity_begin_row(
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_html_transaction_amount_row(
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity->transaction_amount ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_html_net_income_row(
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
	HTML_ROW *html_row;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_begin_balance_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( element_equity_begin_balance ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_display_html_transaction_amount_row(
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_transaction_amount_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_transaction_amount_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_transaction_amount_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money(
			balance_sheet_equity_transaction_amount ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_display_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	HTML_ROW *html_row;

	if ( !income_statement_net_income_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: income_statement_net_income_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			income_statement_net_income_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( income_statement_fetch_net_income ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_display_html_equity_end_row(
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_end_balance_label ),
		1 /* not large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( balance_sheet_equity_end_balance ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_display_html_liability_plus_equity_row(
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_liability_plus_equity_label ),
		1 /* not large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( balance_sheet_liability_plus_equity ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

BALANCE_SHEET_SUBCLASS_OMIT_HTML *
	balance_sheet_subclass_omit_html_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_OMIT_HTML *
		balance_sheet_subclass_omit_html;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
		return (BALANCE_SHEET_SUBCLASS_OMIT_HTML *)0;
	}

	balance_sheet_subclass_omit_html->row_list =
		statement_subclass_omit_html_list_extract_row_list(
			balance_sheet_subclass_omit_html->
				statement_subclass_omit_html_list );

	list_set_list(
		balance_sheet_subclass_omit_html->row_list,
			balance_sheet_subclass_omit_html_equity_row_list(
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
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
			calloc( 1, 
				sizeof( BALANCE_SHEET_SUBCLASS_OMIT_HTML ) ) ) )
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
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LIST *row_list;
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_list = list_new();

	balance_sheet_equity =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		balance_sheet_equity_new(
			element_equity_begin,
			element_equity_current,
			element_liability,
			income_statement_fetch_net_income );

	list_set(
		row_list,
		balance_sheet_subclass_omit_html_equity_begin_row(
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_html_transaction_amount_row(
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity->transaction_amount ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_html_net_income_row(
			income_statement_fetch_net_income,
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
	HTML_ROW *html_row;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_begin_balance_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( element_equity_begin_balance ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_omit_html_transaction_amount_row(
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_transaction_amount_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_transaction_amount_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_transaction_amount_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money(
			balance_sheet_equity_transaction_amount ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_omit_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	HTML_ROW *html_row;

	if ( !income_statement_net_income_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: income_statement_net_income_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			income_statement_net_income_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( income_statement_fetch_net_income ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_omit_html_equity_end_row(
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_end_balance_label ),
		1 /* not large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( balance_sheet_equity_end_balance ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_omit_html_liability_plus_equity_row(
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_liability_plus_equity_label ),
		1 /* not large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( balance_sheet_liability_plus_equity ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

BALANCE_SHEET_SUBCLASS_AGGR_HTML *
	balance_sheet_subclass_aggr_html_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_AGGR_HTML *
		balance_sheet_subclass_aggr_html;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
		return (BALANCE_SHEET_SUBCLASS_AGGR_HTML *)0;
	}

	balance_sheet_subclass_aggr_html->row_list =
		statement_subclass_aggr_html_list_extract_row_list(
			balance_sheet_subclass_aggr_html->
				statement_subclass_aggr_html_list );

	list_set_list(
		balance_sheet_subclass_aggr_html->row_list,
		balance_sheet_subclass_aggr_html_equity_row_list(
			element_equity_begin,
			element_equity_current,
			element_liability,
			income_statement_fetch_net_income,
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
			   sizeof( BALANCE_SHEET_SUBCLASS_AGGR_HTML ) ) ) )
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
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LIST *row_list;
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_list = list_new();

	balance_sheet_equity =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		balance_sheet_equity_new(
			element_equity_begin,
			element_equity_current,
			element_liability,
			income_statement_fetch_net_income );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_html_equity_begin_row(
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_html_transaction_amount_row(
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity->transaction_amount ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_html_net_income_row(
			income_statement_fetch_net_income,
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
	HTML_ROW *html_row;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_begin_balance_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( element_equity_begin_balance ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_aggr_html_transaction_amount_row(
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_transaction_amount_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_transaction_amount_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_transaction_amount_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money(
			balance_sheet_equity_transaction_amount ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_aggr_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	HTML_ROW *html_row;

	if ( !income_statement_net_income_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: income_statement_net_income_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			income_statement_net_income_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( income_statement_fetch_net_income ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_aggr_html_equity_end_row(
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_end_balance_label ),
		1 /* not large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( balance_sheet_equity_end_balance ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

HTML_ROW *balance_sheet_subclass_aggr_html_liability_plus_equity_row(
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity )
{
	HTML_ROW *html_row;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_liability_plus_equity_label ),
		1 /* not large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		timlib_commas_in_money( balance_sheet_liability_plus_equity ),
		0 /* not large_boolean */,
		0 /* bold_boolean */ );

	return html_row;
}

BALANCE_SHEET_HTML *balance_sheet_html_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_HTML *balance_sheet_html;

	if ( !statement
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	balance_sheet_html = balance_sheet_html_calloc();

	balance_sheet_html->element_liability =
		element_seek(
			ELEMENT_LIABILITY,
			statement->element_statement_list );

	if (	statement_subclassification_option ==
		subclassification_display )
	{
		balance_sheet_html->
			balance_sheet_subclass_display_html =
				balance_sheet_subclass_display_html_new(
					statement->element_statement_list,
					statement_prior_year_list,
					element_equity_begin,
					element_equity_current,
					balance_sheet_html->element_liability,
					income_statement_fetch_net_income,
					income_statement_net_income_label );

		if ( !balance_sheet_html->balance_sheet_subclass_display_html )
		{
			free( balance_sheet_html );
			return (BALANCE_SHEET_HTML *)0;
		}

		balance_sheet_html->html_table =
			balance_sheet_html_table(
				statement->caption_subtitle,
				balance_sheet_html->
					balance_sheet_subclass_display_html->
					statement_subclass_display_html_list->
					heading_list,
				balance_sheet_html->
					balance_sheet_subclass_display_html->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		subclassification_omit )
	{
		balance_sheet_html->
			balance_sheet_subclass_omit_html =
				balance_sheet_subclass_omit_html_new(
					statement->element_statement_list,
					statement_prior_year_list,
					element_equity_begin,
					element_equity_current,
					balance_sheet_html->element_liability,
					income_statement_fetch_net_income,
					income_statement_net_income_label );

		if ( ! balance_sheet_html->
			balance_sheet_subclass_omit_html )
		{
			free( balance_sheet_html );
			return (BALANCE_SHEET_HTML *)0;
		}

		balance_sheet_html->html_table =
			balance_sheet_html_table(
				statement->caption_subtitle,
				balance_sheet_html->
					balance_sheet_subclass_omit_html->
					statement_subclass_omit_html_list->
					heading_list,
				balance_sheet_html->
					balance_sheet_subclass_omit_html->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		subclassification_aggregate )
	{
		balance_sheet_html->
			balance_sheet_subclass_aggr_html =
				balance_sheet_subclass_aggr_html_new(
					statement->element_statement_list,
					statement_prior_year_list,
					element_equity_begin,
					element_equity_current,
					balance_sheet_html->element_liability,
					income_statement_fetch_net_income,
					income_statement_net_income_label );

		if ( ! balance_sheet_html->
			balance_sheet_subclass_aggr_html )
		{
			free( balance_sheet_html );
			return (BALANCE_SHEET_HTML *)0;
		}

		balance_sheet_html->html_table =
			balance_sheet_html_table(
				statement->caption_subtitle,
				balance_sheet_html->
					balance_sheet_subclass_aggr_html->
					statement_subclass_aggr_html_list->
					heading_list,
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
			calloc( 1, sizeof( BALANCE_SHEET_HTML ) ) ) )
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
			char *statement_caption_subtitle,
			LIST *heading_list,
			LIST *row_list )
{
	HTML_TABLE *html_table;

	if ( !statement_caption_subtitle
	||   !list_length( heading_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_table =
		html_table_new(
			(char *)0 /* title */,
			statement_caption_subtitle,
			(char *)0 /* sub_sub_title */ );

	html_table->heading_list = heading_list;
	html_table->row_list = row_list;

	return html_table;
}

BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
	balance_sheet_subclass_display_latex_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
		balance_sheet_subclass_display_latex;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
		return (BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *)0;
	}

	balance_sheet_subclass_display_latex->row_list =
		statement_subclass_display_latex_list_extract_row_list(
			balance_sheet_subclass_display_latex->
				statement_subclass_display_latex_list );

	list_set_list(
		balance_sheet_subclass_display_latex->row_list,
			balance_sheet_subclass_display_latex_equity_row_list(
				list_length( statement_prior_year_list )
					/* statement_prior_year_list_length */,
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
				income_statement_net_income_label ) );

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
			sizeof( BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX ) ) ) )
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
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LIST *row_list;
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_list = list_new();

	balance_sheet_equity =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		balance_sheet_equity_new(
			element_equity_begin,
			element_equity_current,
			element_liability,
			income_statement_fetch_net_income );

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_equity_begin_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_transaction_amount_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity->transaction_amount ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_net_income_row(
			statement_prior_year_list_length,
			income_statement_fetch_net_income,
			income_statement_net_income_label ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_equity_end_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity->end_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_display_latex_liability_plus_equity_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity->liability_plus_equity ) );

	return row_list;
}

LATEX_ROW *balance_sheet_subclass_display_latex_equity_begin_row(
			int statement_prior_year_list_length,
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_balance )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_begin_balance_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( element_equity_begin_balance ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_display_latex_transaction_amount_row(
			int statement_prior_year_list_length,
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_transaction_amount_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_transaction_amount_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_transaction_amount_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money(
			balance_sheet_equity_transaction_amount ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_display_latex_net_income_row(
			int statement_prior_year_list_length,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !income_statement_net_income_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: income_statement_net_income_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			income_statement_net_income_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( income_statement_fetch_net_income ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_display_latex_equity_end_row(
			int statement_prior_year_list_length,
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_end_balance_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( balance_sheet_equity_end_balance ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_display_latex_liability_plus_equity_row(
			int statement_prior_year_list_length,
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_liability_plus_equity_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( balance_sheet_liability_plus_equity ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
	balance_sheet_subclass_omit_latex_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
		balance_sheet_subclass_omit_latex;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
		return (BALANCE_SHEET_SUBCLASS_OMIT_LATEX *)0;
	}

	balance_sheet_subclass_omit_latex->row_list =
		statement_subclass_omit_latex_list_extract_row_list(
			balance_sheet_subclass_omit_latex->
				statement_subclass_omit_latex_list );

	list_set_list(
		balance_sheet_subclass_omit_latex->row_list,
			balance_sheet_subclass_omit_latex_equity_row_list(
				list_length( statement_prior_year_list )
					/* statement_prior_year_list_length */,
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
				income_statement_net_income_label ) );

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
			sizeof( BALANCE_SHEET_SUBCLASS_OMIT_LATEX ) ) ) )
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
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LIST *row_list;
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_list = list_new();

	balance_sheet_equity =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		balance_sheet_equity_new(
			element_equity_begin,
			element_equity_current,
			element_liability,
			income_statement_fetch_net_income );

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_equity_begin_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_transaction_amount_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity->transaction_amount ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_net_income_row(
			statement_prior_year_list_length,
			income_statement_fetch_net_income,
			income_statement_net_income_label ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_equity_end_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity->end_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_omit_latex_liability_plus_equity_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity->liability_plus_equity ) );

	return row_list;
}

LATEX_ROW *balance_sheet_subclass_omit_latex_equity_begin_row(
			int statement_prior_year_list_length,
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_balance )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_begin_balance_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( element_equity_begin_balance ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_omit_latex_transaction_amount_row(
			int statement_prior_year_list_length,
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_transaction_amount_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_transaction_amount_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_transaction_amount_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money(
			balance_sheet_equity_transaction_amount ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_omit_latex_net_income_row(
			int statement_prior_year_list_length,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !income_statement_net_income_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: income_statement_net_income_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			income_statement_net_income_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( income_statement_fetch_net_income ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_omit_latex_equity_end_row(
			int statement_prior_year_list_length,
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_end_balance_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( balance_sheet_equity_end_balance ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_omit_latex_liability_plus_equity_row(
			int statement_prior_year_list_length,
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_liability_plus_equity_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( balance_sheet_liability_plus_equity ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
		balance_sheet_subclass_aggr_latex;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
		return (BALANCE_SHEET_SUBCLASS_AGGR_LATEX *)0;
	}

	balance_sheet_subclass_aggr_latex->row_list =
		statement_subclass_aggr_latex_list_extract_row_list(
			balance_sheet_subclass_aggr_latex->
				statement_subclass_aggr_latex_list );

	list_set_list(
		balance_sheet_subclass_aggr_latex->row_list,
			balance_sheet_subclass_aggr_latex_equity_row_list(
				list_length( statement_prior_year_list )
					/* statement_prior_year_list_length */,
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
				income_statement_net_income_label ) );

	return balance_sheet_subclass_aggr_latex;
}

BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_calloc(
			void )
{
	BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
		balance_sheet_subclass_aggr_latex;

	if ( ! ( balance_sheet_subclass_aggr_latex =
		    calloc( 1, sizeof( BALANCE_SHEET_SUBCLASS_AGGR_LATEX ) ) ) )
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
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LIST *row_list;
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( !element_equity_current
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_list = list_new();

	balance_sheet_equity =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		balance_sheet_equity_new(
			element_equity_begin,
			element_equity_current,
			element_liability,
			income_statement_fetch_net_income );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_equity_begin_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_BEGIN_BALANCE_LABEL,
			balance_sheet_equity->begin_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_transaction_amount_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL,
			balance_sheet_equity->transaction_amount ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_net_income_row(
			statement_prior_year_list_length,
			income_statement_fetch_net_income,
			income_statement_net_income_label ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_equity_end_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_END_BALANCE_LABEL,
			balance_sheet_equity->end_balance ) );

	list_set(
		row_list,
		balance_sheet_subclass_aggr_latex_liability_plus_equity_row(
			statement_prior_year_list_length,
			BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL,
			balance_sheet_equity->liability_plus_equity ) );

	return row_list;
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_equity_begin_row(
			int statement_prior_year_list_length,
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_balance )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_begin_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_begin_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_begin_balance_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( element_equity_begin_balance ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_transaction_amount_row(
			int statement_prior_year_list_length,
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_transaction_amount_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_transaction_amount_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_transaction_amount_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money(
			balance_sheet_equity_transaction_amount ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_net_income_row(
			int statement_prior_year_list_length,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !income_statement_net_income_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: income_statement_net_income_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			income_statement_net_income_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( income_statement_fetch_net_income ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_equity_end_row(
			int statement_prior_year_list_length,
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_end_balance_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_end_balance_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_end_balance_label ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( balance_sheet_equity_end_balance ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_liability_plus_equity_row(
			int statement_prior_year_list_length,
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !balance_sheet_liability_plus_equity_label )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: balance_sheet_liability_plus_equity_label is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			balance_sheet_liability_plus_equity_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		timlib_commas_in_money( balance_sheet_liability_plus_equity ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

BALANCE_SHEET_LATEX *
	balance_sheet_latex_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
	BALANCE_SHEET_LATEX *balance_sheet_latex;

	if ( !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !statement
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	balance_sheet_latex = balance_sheet_latex_calloc();

	balance_sheet_latex->latex =
		latex_new(
			tex_filename,
			dvi_filename,
			working_directory,
			statement_pdf_landscape_boolean,
			statement_logo_filename );


	if (	statement_subclassification_option ==
		subclassification_display )
	{
		balance_sheet_latex->balance_sheet_subclass_display_latex =
			balance_sheet_subclass_display_latex_new(
				statement->element_statement_list,
				statement_prior_year_list,
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
				income_statement_net_income_label );

		if ( !balance_sheet_latex->
			balance_sheet_subclass_display_latex )
		{
			free( balance_sheet_latex );
			return (BALANCE_SHEET_LATEX *)0;
		}

		list_set(
			balance_sheet_latex->latex->table_list,
			balance_sheet_latex_table(
				statement->caption,
				balance_sheet_latex->
					balance_sheet_subclass_display_latex->
					statement_subclass_display_latex_list->
					heading_list,
				balance_sheet_latex->
					balance_sheet_subclass_display_latex->
					row_list ) );
	}
	else
	if (	statement_subclassification_option ==
		subclassification_omit )
	{
		balance_sheet_latex->balance_sheet_subclass_omit_latex =
			balance_sheet_subclass_omit_latex_new(
				statement->element_statement_list,
				statement_prior_year_list,
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
				income_statement_net_income_label );

		if ( !balance_sheet_latex->
			balance_sheet_subclass_omit_latex )
		{
			free( balance_sheet_latex );
			return (BALANCE_SHEET_LATEX *)0;
		}

		list_set(
			balance_sheet_latex->latex->table_list,
			balance_sheet_latex_table(
				statement->caption,
				balance_sheet_latex->
					balance_sheet_subclass_omit_latex->
					statement_subclass_omit_latex_list->
					heading_list,
				balance_sheet_latex->
					balance_sheet_subclass_omit_latex->
					row_list ) );
	}
	else
	if (	statement_subclassification_option ==
		subclassification_aggregate )
	{
		balance_sheet_latex->balance_sheet_subclass_aggr_latex =
			balance_sheet_subclass_aggr_latex_new(
				statement->element_statement_list,
				statement_prior_year_list,
				element_equity_begin,
				element_equity_current,
				element_liability,
				income_statement_fetch_net_income,
				income_statement_net_income_label );

		if ( !balance_sheet_latex->
			balance_sheet_subclass_aggr_latex )
		{
			free( balance_sheet_latex );
			return (BALANCE_SHEET_LATEX *)0;
		}

		list_set(
			balance_sheet_latex->latex->table_list,
			balance_sheet_latex_table(
				statement->caption,
				balance_sheet_latex->
					balance_sheet_subclass_aggr_latex->
					statement_subclass_aggr_latex_list->
					heading_list,
				balance_sheet_latex->
					balance_sheet_subclass_aggr_latex->
					row_list ) );
	}

	return balance_sheet_latex;
}

BALANCE_SHEET_LATEX *balance_sheet_latex_calloc( void )
{
	BALANCE_SHEET_LATEX *balance_sheet_latex;

	if ( ! ( balance_sheet_latex =
			calloc( 1, sizeof( BALANCE_SHEET_LATEX ) ) ) )
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
			char *statement_caption,
			LIST *heading_list,
			LIST *row_list )
{
	LATEX_TABLE *latex_table;

	if ( !statement_caption
	||   !list_length( heading_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	latex_table = latex_table_new( statement_caption );
	latex_table->heading_list = heading_list;
	latex_table->row_list = row_list;

	return latex_table;
}

BALANCE_SHEET_PDF *balance_sheet_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label,
			pid_t process_id )
{
	BALANCE_SHEET_PDF *balance_sheet_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !statement
	||   !income_statement_net_income_label
	||   !process_id )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	balance_sheet_pdf = balance_sheet_pdf_calloc();

	balance_sheet_pdf->element_liability =
		element_seek(
			ELEMENT_LIABILITY,
			statement->element_statement_list );

	balance_sheet_pdf->statement_pdf_landscape_boolean =
		statement_pdf_landscape_boolean(
		list_length( statement_prior_year_list )
			/* statement_prior_year_list_length */ );

	balance_sheet_pdf->statement_link =
		statement_link_new(
			application_name,
			process_name,
			document_root_directory,
			statement->transaction_begin_date_string,
			statement->transaction_as_of_date,
			(char *)0 /* statement_pdf_preclose_key */,
			process_id );

	balance_sheet_pdf->balance_sheet_latex =
		balance_sheet_latex_new(
			statement_subclassification_option,
			balance_sheet_pdf->statement_link->tex_filename,
			balance_sheet_pdf->statement_link->dvi_filename,
			balance_sheet_pdf->statement_link->working_directory,
			balance_sheet_pdf->statement_pdf_landscape_boolean,
			statement->logo_filename,
			statement,
			statement_prior_year_list,
			element_equity_begin,
			element_equity_current,
			balance_sheet_pdf->element_liability,
			income_statement_fetch_net_income,
			income_statement_net_income_label );

	return balance_sheet_pdf;
}

BALANCE_SHEET_PDF *balance_sheet_pdf_calloc( void )
{
	BALANCE_SHEET_PDF *balance_sheet_pdf;

	if ( ! ( balance_sheet_pdf =
			calloc( 1, sizeof( BALANCE_SHEET_PDF ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_pdf;
}

BALANCE_SHEET *balance_sheet_fetch(
			char *argv_0,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			char *document_root_directory,
			char *as_of_date,
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
	||   !document_root_directory
	||   !as_of_date
	||   !subclassification_option_string
	||   !output_medium_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	balance_sheet = balance_sheet_calloc();

	balance_sheet->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassification_option_string );

	balance_sheet->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	balance_sheet->transaction_as_of_date =
		transaction_as_of_date(
			TRANSACTION_TABLE,
			as_of_date );

	if ( !balance_sheet->transaction_as_of_date )
	{
		free( balance_sheet );
		return (BALANCE_SHEET *)0;
	}

	balance_sheet->transaction_begin_date_string =
		transaction_begin_date_string(
			TRANSACTION_TABLE,
			balance_sheet->transaction_as_of_date );

	if ( !balance_sheet->transaction_begin_date_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: transaction_begin_date_string(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			balance_sheet->transaction_as_of_date );
		exit( 1 );
	}

	balance_sheet->element_name_list =
		balance_sheet_element_name_list(
			ELEMENT_ASSET,
			ELEMENT_LIABILITY );

	balance_sheet->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSE_TIME,
			balance_sheet->transaction_as_of_date );

	balance_sheet->transaction_date_time_closing =
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			balance_sheet->transaction_as_of_date,
			balance_sheet->transaction_closing_entry_exists
				/* preclose_time_boolean */ );

	balance_sheet->statement =
		statement_fetch(
			application_name,
			process_name,
			balance_sheet->element_name_list,
			balance_sheet->transaction_begin_date_string,
			balance_sheet->transaction_as_of_date,
			balance_sheet->transaction_date_time_closing,
			0 /* not fetch_transaction */ );

	balance_sheet->element_equity_current =
		element_statement_fetch(
			ELEMENT_EQUITY,
			balance_sheet->transaction_date_time_closing,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	if ( !balance_sheet->element_equity_current )
	{
		free( balance_sheet );
		return (BALANCE_SHEET *)0;
	}

	balance_sheet->element_equity_current->sum =
		element_sum(
			balance_sheet->element_equity_current );

	if ( balance_sheet->statement_output_medium == output_table )
	{
		element_account_transaction_count_set(
			balance_sheet->statement->element_statement_list,
			balance_sheet->transaction_begin_date_string,
			balance_sheet->transaction_date_time_closing );

		element_account_action_string_set(
			balance_sheet->statement->element_statement_list,
			application_name,
			session_key,
			login_name,
			role_name,
			balance_sheet->transaction_begin_date_string,
			balance_sheet->transaction_date_time_closing );
	}

	if ( prior_year_count )
	{
		balance_sheet->statement_prior_year_list =
			statement_prior_year_list(
				balance_sheet->element_name_list,
				balance_sheet->transaction_date_time_closing,
				prior_year_count,
				balance_sheet->statement );
	}

	balance_sheet->prior_date_time_closing =
		balance_sheet_prior_date_time_closing(
			balance_sheet->transaction_begin_date_string );

	if ( balance_sheet->prior_date_time_closing )
	{
		balance_sheet->element_equity_begin =
			element_statement_fetch(
				ELEMENT_EQUITY,
				balance_sheet->prior_date_time_closing,
				1 /* fetch_subclassification_list */,
				1 /* fetch_account_list */,
				1 /* fetch_journal_latest */,
				0 /* not fetch_transaction */ );

		balance_sheet->element_equity_begin->sum =
			element_sum(
				balance_sheet->element_equity_begin );
	}

	balance_sheet->income_statement_fetch_net_income =
		income_statement_fetch_net_income(
			balance_sheet->transaction_date_time_closing );

	balance_sheet->income_statement_net_income_label =
		income_statement_net_income_label(
			argv_0 );

	if (	balance_sheet->statement_subclassification_option ==
		subclassification_omit )
	{
		element_list_account_statement_list_set(
			balance_sheet->statement->element_statement_list );
	}

	if ( balance_sheet->statement_output_medium == output_PDF )
	{
		balance_sheet->balance_sheet_pdf =
			balance_sheet_pdf_new(
				application_name,
				process_name,
				document_root_directory,
				balance_sheet->
					statement_subclassification_option,
				balance_sheet->statement,
				balance_sheet->statement_prior_year_list,
				balance_sheet->element_equity_begin,
				balance_sheet->element_equity_current,
				balance_sheet->
					income_statement_fetch_net_income,
				balance_sheet->
					income_statement_net_income_label,
				getpid() /* process_id */ );
	}
	else
	if ( balance_sheet->statement_output_medium == output_table )
	{
		balance_sheet->balance_sheet_html =
			balance_sheet_html_new(
				balance_sheet->
					statement_subclassification_option,
				balance_sheet->statement,
				balance_sheet->statement_prior_year_list,
				balance_sheet->element_equity_begin,
				balance_sheet->element_equity_current,
				balance_sheet->
					income_statement_fetch_net_income,
				balance_sheet->
					income_statement_net_income_label );
	}

	return balance_sheet;
}

BALANCE_SHEET *balance_sheet_calloc( void )
{
	BALANCE_SHEET *balance_sheet;

	if ( ! ( balance_sheet = calloc( 1, sizeof( BALANCE_SHEET ) ) ) )
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

char *balance_sheet_prior_date_time_closing(
			char *transaction_begin_date_string )
{
	DATE *prior =
		/* ------------------- */
		/* Trims trailing time */
		/* ------------------- */
		date_yyyy_mm_dd_new(
			transaction_begin_date_string );

	date_decrement_second( prior, 1 );

	return date_display19( prior );
}

BALANCE_SHEET_EQUITY *balance_sheet_equity_calloc( void )
{
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	if ( ! ( balance_sheet_equity =
			calloc( 1, sizeof( BALANCE_SHEET_EQUITY ) ) ) )
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
			double income_statement_fetch_net_income )
{
	BALANCE_SHEET_EQUITY *balance_sheet_equity;

	balance_sheet_equity = balance_sheet_equity_calloc();

	balance_sheet_equity->begin_balance =
		balance_sheet_equity_begin_balance(
			element_equity_begin );

	balance_sheet_equity->current_balance =
		balance_sheet_equity_current_balance(
			element_equity_current );

	balance_sheet_equity->transaction_amount =
		balance_sheet_equity_transaction_amount(
			balance_sheet_equity->current_balance,
			balance_sheet_equity->begin_balance );

	balance_sheet_equity->end_balance =
		balance_sheet_equity_end_balance(
			balance_sheet_equity->current_balance,
			income_statement_fetch_net_income );

	balance_sheet_equity->liability_balance =
		balance_sheet_equity_liability_balance(
			element_liability );

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
			double equity_current_balance,
			double equity_begin_balance )
{
	return
	equity_current_balance -
	equity_begin_balance;
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

