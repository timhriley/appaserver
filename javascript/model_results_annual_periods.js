// model_results_annual_periods.js
// -------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_annual_periods( current_element )
{
	var aggregate_period_element;
	var element;
	var aggregate_period_value;

	// Temporary, until completed
	// --------------------------
	// element = timlib_get_element( "output_medium_0" );
	// element.disabled = 1;

	aggregate_period_element = timlib_get_element( "aggregate_period_0" );

	aggregate_period_value =
		timlib_get_drop_down_element_value(
			aggregate_period_element.options );

	if ( aggregate_period_value == 'seasonally' )
	{
		element = timlib_get_element( "season_0" );
		element.disabled = 0;
		element = timlib_get_element( "month_0" );
		element.disabled = 1;
		element = timlib_get_element( "from_containing_date_0" );
		element.disabled = 1;
	}
	else
	if ( aggregate_period_value == 'monthly' )
	{
		element = timlib_get_element( "season_0" );
		element.disabled = 1;
		element = timlib_get_element( "month_0" );
		element.disabled = 0;
		element = timlib_get_element( "from_containing_date_0" );
		element.disabled = 1;
	}
	else
	if ( aggregate_period_value == 'weekly' )
	{
		element = timlib_get_element( "season_0" );
		element.disabled = 1;
		element = timlib_get_element( "month_0" );
		element.disabled = 1;
		element = timlib_get_element( "from_containing_date_0" );
		element.disabled = 0;
	}
	else
	if ( aggregate_period_value == 'daily' )
	{
		element = timlib_get_element( "season_0" );
		element.disabled = 1;
		element = timlib_get_element( "month_0" );
		element.disabled = 1;
		element = timlib_get_element( "from_containing_date_0" );
		element.disabled = 0;
	}
	else
	{
		element = timlib_get_element( "season_0" );
		element.disabled = 1;
		element = timlib_get_element( "month_0" );
		element.disabled = 1;
		element = timlib_get_element( "from_containing_date_0" );
		element.disabled = 1;
	}

	return true;
} // post_change_annual_periods()

