/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction_date.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and  available software. Visit appaserver.org		*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "sql.h"
#include "appaserver.h"
#include "environ.h"
#include "application.h"
#include "appaserver_error.h"
#include "float.h"
#include "transaction.h"
#include "transaction_date.h"

char *transaction_date_where_string(
		const char *memo,
		char *date_string,
		char *time_string )
{
	static char where_string[ 128 ];

	if ( !date_string
	||   !time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where_string,
		"transaction_date_time = '%s' and "
		"memo = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time_string(
			date_string,
			time_string ),
		memo );

	return where_string;
}

boolean transaction_date_time_boolean(
		const char *transaction_table,
		char *transaction_date_time )
{
	char *system_string;
	char *where_string;
	char *string;
	boolean return_value;

	if ( !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "transaction_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time_where_string(
			transaction_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_date_count_system_string(
			transaction_table,
			where_string );

	string = string_pipe( system_string );
	return_value = (boolean)( atoi( string ) == 1 );
	free( string );

	return return_value;
}

boolean transaction_date_close_boolean(
		const char *transaction_table,
		const char *transaction_date_close_time,
		const char *transaction_close_memo,
		char *transaction_date_as_of )
{
	char *close_where_string;
	char *count_system_string;
	char *string;
	boolean return_value;

	if ( !transaction_date_as_of )
	{
		char message[ 128 ];

		sprintf(message, "transaction_date_as_of is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_where_string(
			transaction_close_memo,
			transaction_date_as_of
				/* date_string */,
			(char *)transaction_date_close_time
				/* time_string */ );

	count_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_date_count_system_string(
			transaction_table,
			close_where_string );

	string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe(
			count_system_string );

	return_value = (boolean)( atoi( string ) == 1 );

	free( count_system_string );
	free( string );

	return return_value;
}

boolean transaction_date_reverse_boolean(
		const char *transaction_table,
		const char *transaction_date_reverse_time,
		const char *transaction_reverse_memo,
		char *reverse_date_string )
{
	char *where_string;
	char *count_system_string;
	char *string;
	boolean return_value;

	if ( !reverse_date_string )
	{
		char message[ 128 ];

		sprintf(message, "reverse_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_where_string(
			transaction_reverse_memo,
			reverse_date_string,
			(char *)transaction_date_reverse_time
				/* time_string */ );

	count_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_date_count_system_string(
			transaction_table,
			where_string );

	string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe(
			count_system_string );

	return_value = (boolean)( atoi( string ) == 1 );

	free( count_system_string );
	free( string );

	return return_value;
}

char *transaction_date_close_date_time_string(
		const char *transaction_date_preclose_time,
		const char *transaction_date_close_time,
		char *transaction_date_as_of,
		boolean preclose_time_boolean )
{
	char *date_time_string;

	if ( !transaction_date_as_of )
	{
		char message[ 128 ];

		sprintf(message, "transaction_date_as_of is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( preclose_time_boolean )
	{
		date_time_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_string(
				transaction_date_as_of,
				(char *)transaction_date_preclose_time );
	}
	else
	{
		date_time_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_string(
				transaction_date_as_of,
				(char *)transaction_date_close_time );
	}

	return strdup( date_time_string );
}

char *transaction_date_reverse_date_time_string(
		const char *transaction_date_reverse_time,
		char *reverse_date_string )
{
	if ( !reverse_date_string )
	{
		char message[ 128 ];

		sprintf(message, "reverse_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time_string(
			reverse_date_string,
			(char *)transaction_date_reverse_time ) );
}

boolean transaction_date_time_changed(
		char *preupdate_transaction_date_time )
{
	if ( !preupdate_transaction_date_time
	||   !*preupdate_transaction_date_time
	||   strcmp(	preupdate_transaction_date_time,
			"preupdate_transaction_date_time" ) == 0 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

char *transaction_date_as_of(
		const char *transaction_table,
		char *as_of_date_string )
{
	if ( string_populated_boolean(
		TRANSACTION_DATE_AS_OF_DATE_FILLER,
		as_of_date_string ) )
	{
		return as_of_date_string;
	}
	else
	{
		char as_of_date[ 16 ];
		char *date_time_maximum_string;

		date_time_maximum_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			transaction_date_time_maximum_string(
				transaction_table,
				(char *)0 /* where_string */ );

		if ( !date_time_maximum_string ) return NULL;

		return
		strdup(
			column(	as_of_date,
				0,
				date_time_maximum_string ) );
	}
}

char *transaction_date_time_maximum_string(
		const char *transaction_table,
		char *where_string )
{
	char system_string[ 1024 ];
	char *select;
	char *string;

	if ( !where_string ) where_string = "1 = 1";

	select = "max( transaction_date_time )";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		transaction_table,
		where_string );

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string = string_pipe( system_string );

	if ( string && !*string ) string = (char *)0;

	return string;
}

DATE *transaction_date_prior_close_date(
		const char *transaction_date_close_time,
		const char *transaction_close_memo,
		const char *transaction_table,
		char *transaction_date_as_of )
{
	char *select;
	char *date_time_string;
	char where_string[ 128 ];
	char system_string[ 256 ];
	char *string;

	if ( !transaction_date_as_of )
	{
		char message[ 128 ];

		sprintf(message, "transaction_date_as_of is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select = "max( transaction_date_time )";

	date_time_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time_string(
			transaction_date_as_of
				/* date_string */,
			(char *)transaction_date_close_time
				/* time_string */ );

	sprintf(where_string,
		"memo = '%s' and transaction_date_time < '%s'",
		transaction_close_memo,
		date_time_string );

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		transaction_table,
		where_string );

	string = string_pipe( system_string );

	if ( string )
	{
		return
		date_yyyy_mm_dd_new( string );
	}
	else
	{
		return NULL;
	}
}

char *transaction_date_minimum_string( const char *transaction_table )
{
	char *select;
	char system_string[ 1024 ];
	char minimum_string[ 16 ];
	char *string;

	select = "min( transaction_date_time )";

	sprintf(system_string,
		"select.sh \"%s\" %s",
		select,
		transaction_table );

	/* Returns heap memory or null */
	/* --------------------------- */
	string = string_pipe( system_string );

	if ( string )
	{
		return
		strdup( column( minimum_string, 0, string )  );
	}
	else
	{
		return NULL;
	}
}

char *transaction_date_begin_date_string(
		const char *transaction_table,
		char *transaction_date_as_of )
{
	DATE *prior_close_date = {0};

	prior_close_date =
		transaction_date_prior_close_date(
			TRANSACTION_DATE_CLOSE_TIME,
			TRANSACTION_CLOSE_MEMO,
			transaction_table,
			transaction_date_as_of );

	if ( prior_close_date )
	{
		date_increment_days(
			prior_close_date,
			1.0 );

		return
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		date_yyyy_mm_dd_string(
			prior_close_date );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	transaction_date_minimum_string(
		transaction_table );
}

char *transaction_date_earlier_date_time(
		char *transaction_date_time,
		char *preupdate_transaction_date_time )
{
	char *earlier_date_time;

	if ( !transaction_date_time_changed(
		transaction_date_time ) )
	{
		earlier_date_time = transaction_date_time;
	}
	else
	if ( string_strcmp(
		transaction_date_time,
		preupdate_transaction_date_time ) <= 0 )
	{
		earlier_date_time = transaction_date_time;
	}
	else
	{
		earlier_date_time = preupdate_transaction_date_time;
	}

	return earlier_date_time;
}

char *transaction_date_time_memo_maximum_string(
		const char *transaction_table,
		const char *transaction_close_memo )
{
	char where_string[ 128 ];

	snprintf(
		where_string,
		sizeof ( where_string ),
		"memo = '%s'",
		transaction_close_memo );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	transaction_date_time_maximum_string(
		transaction_table,
		where_string );
}

char *transaction_date_end_date_time_string(
		char *end_date_string,
		char *time_string )
{
	static char end_date_time_string[ 32 ];

	if ( !end_date_string
	||   !time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(end_date_time_string,
		"%s %s",
		end_date_string,
		time_string );

	return end_date_time_string;
}

TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
	transaction_date_close_nominal_undo_new(
		const char *transaction_table,
		const char *transaction_close_memo )
{
	TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
		transaction_date_close_nominal_undo;

	transaction_date_close_nominal_undo =
		transaction_date_close_nominal_undo_calloc();

	transaction_date_close_nominal_undo->
		transaction_date_time_memo_maximum_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			transaction_date_time_memo_maximum_string(
				transaction_table,
				transaction_close_memo );

	return transaction_date_close_nominal_undo;
}

TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
	transaction_date_close_nominal_undo_calloc(
		void )
{
	TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
		transaction_date_close_nominal_undo;

	if ( ! ( transaction_date_close_nominal_undo =
		   calloc(
			1,
			sizeof ( TRANSACTION_DATE_CLOSE_NOMINAL_UNDO ) ) ) ) 
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction_date_close_nominal_undo;
}

TRANSACTION_DATE_CLOSE_NOMINAL_DO *
	transaction_date_close_nominal_do_new(
		char *as_of_date_string )
{
	TRANSACTION_DATE_CLOSE_NOMINAL_DO *
		transaction_date_close_nominal_do;

	if ( !as_of_date_string )
	{
		char message[ 128 ];

		sprintf(message, "as_of_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_date_close_nominal_do =
		transaction_date_close_nominal_do_calloc();

	transaction_date_close_nominal_do->
		transaction_date_close_boolean =
			transaction_date_close_boolean(
				TRANSACTION_TABLE,
				TRANSACTION_DATE_CLOSE_TIME,
				TRANSACTION_CLOSE_MEMO,
				as_of_date_string );

	if ( !transaction_date_close_nominal_do->
		transaction_date_close_boolean )
	{
	    transaction_date_close_nominal_do->
		transaction_date_close_date_time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			transaction_date_close_date_time_string(
				TRANSACTION_DATE_PRECLOSE_TIME,
				TRANSACTION_DATE_CLOSE_TIME,
				as_of_date_string,
				0 /* not preclose_time_boolean */ );
	}

	return transaction_date_close_nominal_do;
}

TRANSACTION_DATE_CLOSE_NOMINAL_DO *
	transaction_date_close_nominal_do_calloc(
		void )
{
	TRANSACTION_DATE_CLOSE_NOMINAL_DO *
		transaction_date_close_nominal_do;

	if ( ! ( transaction_date_close_nominal_do =
		   calloc(
			1,
			sizeof ( TRANSACTION_DATE_CLOSE_NOMINAL_DO ) ) ) ) 
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction_date_close_nominal_do;
}

TRANSACTION_DATE_STATEMENT *
	transaction_date_statement_new(
		char *as_of_date_string )
{
	TRANSACTION_DATE_STATEMENT *transaction_date_statement;

	if ( !as_of_date_string )
	{
		char message[ 128 ];

		sprintf(message, "as_of_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_date_statement = transaction_date_statement_calloc();

	transaction_date_statement->
		transaction_date_as_of =
			/* ----------------------------------------------- */
			/* Returns as_of_date_string, heap memory, or null */
			/* ----------------------------------------------- */
			transaction_date_as_of(
				TRANSACTION_TABLE,
				as_of_date_string );

	if ( !transaction_date_statement->
		transaction_date_as_of )
	{
		return transaction_date_statement;
	}

	transaction_date_statement->
		transaction_date_begin_date_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			transaction_date_begin_date_string(
				TRANSACTION_TABLE,
				transaction_date_statement->
					transaction_date_as_of );

	if ( !transaction_date_statement->
		transaction_date_begin_date_string )
	{
		char message[ 128 ];

		sprintf(message,
		"transaction_date_begin_date_string(%s) returned empty.",
			transaction_date_statement->
				transaction_date_as_of );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_date_statement->transaction_date_close_boolean =
		transaction_date_close_boolean(
			TRANSACTION_TABLE,
			TRANSACTION_DATE_CLOSE_TIME,
			TRANSACTION_CLOSE_MEMO,
			transaction_date_statement->
				transaction_date_as_of );

	transaction_date_statement->end_date_time_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_date_close_date_time_string(
			TRANSACTION_DATE_PRECLOSE_TIME,
			TRANSACTION_DATE_CLOSE_TIME,
			transaction_date_statement->
				transaction_date_as_of,
			transaction_date_statement->
				transaction_date_close_boolean
				/* preclose_time_boolean */ );

	transaction_date_statement->
		prior_end_date_time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			transaction_date_prior_end_date_time_string(
				transaction_date_statement->
					transaction_date_begin_date_string );

	return transaction_date_statement;
}

TRANSACTION_DATE_STATEMENT *
	transaction_date_statement_calloc(
		void )
{
	TRANSACTION_DATE_STATEMENT *transaction_date_statement;

	if ( ! ( transaction_date_statement =
			calloc( 1,
				sizeof ( TRANSACTION_DATE_STATEMENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction_date_statement;
}

char *transaction_date_time_string(
		char *date_string,
		char *time_string )
{
	static char date_time_string[ 32 ];

	if ( !date_string
	||   !time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(date_time_string,
		"%s %s",
		date_string,
		time_string );

	return date_time_string;
}

TRANSACTION_DATE_TRIAL_BALANCE *
	transaction_date_trial_balance_new(
		char *as_of_date_string )
{
	TRANSACTION_DATE_TRIAL_BALANCE *
		transaction_date_trial_balance;

	transaction_date_trial_balance =
		transaction_date_trial_balance_calloc();

	transaction_date_trial_balance->
		transaction_date_as_of =
			/* ----------------------------------------------- */
			/* Returns as_of_date_string, heap memory, or null */
			/* ----------------------------------------------- */
			transaction_date_as_of(
				TRANSACTION_TABLE,
				as_of_date_string );

	if ( !transaction_date_trial_balance->
		transaction_date_as_of )
	{
		return transaction_date_trial_balance;
	}

	transaction_date_trial_balance->
		transaction_date_begin_date_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			transaction_date_begin_date_string(
				TRANSACTION_TABLE,
				transaction_date_trial_balance->
					transaction_date_as_of );

	if ( !transaction_date_trial_balance->
		transaction_date_begin_date_string )
	{
		char message[ 128 ];

		sprintf(message,
		"transaction_date_begin_date_string(%s) returned empty.",
			transaction_date_trial_balance->
				transaction_date_as_of );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_date_trial_balance->
		transaction_date_close_boolean =
			transaction_date_close_boolean(
				TRANSACTION_TABLE,
				TRANSACTION_DATE_CLOSE_TIME,
				TRANSACTION_CLOSE_MEMO,
				transaction_date_trial_balance->
					transaction_date_as_of );

	if ( transaction_date_trial_balance->
		transaction_date_close_boolean )
	{
		transaction_date_trial_balance->
			preclose_end_date_time_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				transaction_date_close_date_time_string(
					TRANSACTION_DATE_PRECLOSE_TIME,
					TRANSACTION_DATE_CLOSE_TIME,
					transaction_date_trial_balance->
						transaction_date_as_of,
					1 /* preclose_time_boolean */ );
	}

	transaction_date_trial_balance->
		end_date_time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			transaction_date_close_date_time_string(
				TRANSACTION_DATE_PRECLOSE_TIME,
				TRANSACTION_DATE_CLOSE_TIME,
				transaction_date_trial_balance->
					transaction_date_as_of,
				0 /* not preclose_time_boolean */ );

	return transaction_date_trial_balance;
}

TRANSACTION_DATE_TRIAL_BALANCE *
	transaction_date_trial_balance_calloc(
		void )
{
	TRANSACTION_DATE_TRIAL_BALANCE *
		transaction_date_trial_balance;

	if ( ! ( transaction_date_trial_balance =
			calloc( 1,
				sizeof ( TRANSACTION_DATE_TRIAL_BALANCE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction_date_trial_balance;
}

char *transaction_date_count_system_string(
		const char *transaction_table,
		char *where_string )
{
	char *select;
	char system_string[ 256 ];

	if ( !where_string ) where_string = "1 = 1";

	select = "count(1)";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		transaction_table,
		where_string );

	return strdup( system_string );
}

char *transaction_date_time_where_string( char *transaction_date_time )
{
	static char where_string[ 128 ];

	if ( !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "transaction_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where_string,
		sizeof ( where_string ),
		"transaction_date_time = '%s'",
		transaction_date_time );

	return where_string;
}

char *transaction_date_prior_end_date_time_string(
		char *transaction_date_begin_date_string )
{
	DATE *prior =
		/* ------------------- */
		/* Trims trailing time */
		/* ------------------- */
		date_yyyy_mm_dd_new(
			transaction_date_begin_date_string );

	date_decrement_second( prior, 1 );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_display19( prior );
}

TRANSACTION_DATE_REVERSE_NOMINAL_UNDO *
	transaction_date_reverse_nominal_undo_new(
		const char *transaction_table,
		const char *transaction_reverse_memo )
{
	TRANSACTION_DATE_REVERSE_NOMINAL_UNDO *
		transaction_date_reverse_nominal_undo;

	transaction_date_reverse_nominal_undo =
		transaction_date_reverse_nominal_undo_calloc();

	transaction_date_reverse_nominal_undo->
		transaction_date_time_memo_maximum_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			transaction_date_time_memo_maximum_string(
				transaction_table,
				transaction_reverse_memo );

	return transaction_date_reverse_nominal_undo;
}

TRANSACTION_DATE_REVERSE_NOMINAL_UNDO *
	transaction_date_reverse_nominal_undo_calloc(
		void )
{
	TRANSACTION_DATE_REVERSE_NOMINAL_UNDO *
		transaction_date_reverse_nominal_undo;

	if ( ! ( transaction_date_reverse_nominal_undo =
		   calloc(
			1,
			sizeof ( TRANSACTION_DATE_REVERSE_NOMINAL_UNDO ) ) ) ) 
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction_date_reverse_nominal_undo;
}

TRANSACTION_DATE_REVERSE_NOMINAL_DO *
	transaction_date_reverse_nominal_do_new(
		char *reverse_date_string )
{
	TRANSACTION_DATE_REVERSE_NOMINAL_DO *
		transaction_date_reverse_nominal_do;

	if ( !reverse_date_string )
	{
		char message[ 128 ];

		sprintf(message, "reverse_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_date_reverse_nominal_do =
		transaction_date_reverse_nominal_do_calloc();

	transaction_date_reverse_nominal_do->
		transaction_date_reverse_boolean =
			transaction_date_reverse_boolean(
				TRANSACTION_TABLE,
				TRANSACTION_DATE_REVERSE_TIME,
				TRANSACTION_REVERSE_MEMO,
				reverse_date_string );

	if ( !transaction_date_reverse_nominal_do->
		transaction_date_reverse_boolean )
	{
	    transaction_date_reverse_nominal_do->
		transaction_date_reverse_date_time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			transaction_date_reverse_date_time_string(
				TRANSACTION_DATE_REVERSE_TIME,
				reverse_date_string );
	}

	return transaction_date_reverse_nominal_do;
}

TRANSACTION_DATE_REVERSE_NOMINAL_DO *
	transaction_date_reverse_nominal_do_calloc(
		void )
{
	TRANSACTION_DATE_REVERSE_NOMINAL_DO *
		transaction_date_reverse_nominal_do;

	if ( ! ( transaction_date_reverse_nominal_do =
		   calloc(
			1,
			sizeof ( TRANSACTION_DATE_REVERSE_NOMINAL_DO ) ) ) ) 
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return transaction_date_reverse_nominal_do;
}

