/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_fetch.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

PURCHASE_FETCH *purchase_fetch_new(
		const char *purchase_select,
		const char *purchase_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time )
{
}

PURCHASE_FETCH *purchase_fetch_calloc( void )
{
	PURCHASE_FETCH *purchase_fetch;

	if ( ! ( purchase_fetch = calloc( 1, sizeof ( PURCHASE_FETCH ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return purchase_fetch;
}

