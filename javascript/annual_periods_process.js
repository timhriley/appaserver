// $APPASERVER_HOME/javascript/annual_periods_process.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function post_change_annual_periods( current_element )
{
	var aggregate_period_element;
	var element;
	var aggregate_period_value;
	var containing_element;
	var containing_value;

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
		element = timlib_get_element( "from_dynamic_ending_date_0" );
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
		element = timlib_get_element( "from_dynamic_ending_date_0" );
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
		element = timlib_get_element( "from_dynamic_ending_date_0" );
		element.disabled = 1;
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
		element = timlib_get_element( "from_dynamic_ending_date_0" );
		element.disabled = 1;
	}
	else
	if ( aggregate_period_value == 'dynamically' )
	{
		element = timlib_get_element( "season_0" );
		element.disabled = 1;
		element = timlib_get_element( "month_0" );
		element.disabled = 1;
		element = timlib_get_element( "from_containing_date_0" );
		element.disabled = 0;
		element = timlib_get_element( "from_dynamic_ending_date_0" );
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
		element = timlib_get_element( "from_dynamic_ending_date_0" );
		element.disabled = 1;
	}

	// Set delta and exceedance_format_yn
	// ----------------------------------
	element = timlib_get_element( "delta_0" );
	element.disabled = 0;
	element = timlib_get_element( "from_exceedance_format_yn_0" );
	element.disabled = 0;

	if ( aggregate_period_value == 'select' )
	{
		element = timlib_get_element( "delta_0" );
		element.disabled = 1;
	}
	else
	if ( aggregate_period_value == 'annually' )
	{
		// stub
	}
	else
	if ( aggregate_period_value == 'seasonally' )
	{
		containing_element = timlib_get_element( "season_0" );

		containing_value =
			timlib_get_drop_down_element_value(
				containing_element.options );

		if ( containing_value == 'select' )
		{
			element = timlib_get_element( "delta_0" );
			element.disabled = 1;
			element =
				timlib_get_element(
					"from_exceedance_format_yn_0" );
			element.disabled = 1;

		}
	}
	else
	if ( aggregate_period_value == 'monthly' )
	{
		containing_element = timlib_get_element( "month_0" );

		containing_value =
			timlib_get_drop_down_element_value(
				containing_element.options );

		if ( containing_value == 'select'
		||   containing_value == ''
		||   containing_value == 'undefined' )
		{
			element = timlib_get_element( "delta_0" );
			element.disabled = 1;
			element =
				timlib_get_element(
					"from_exceedance_format_yn_0" );
			element.disabled = 1;
		}
	}
	else
	if ( aggregate_period_value == 'weekly'
	||   aggregate_period_value == 'daily' )
	{
		containing_element =
			timlib_get_element( "from_containing_date_0" );

		containing_value = containing_element.value;

		if ( containing_value == '' )
		{
			element = timlib_get_element( "delta_0" );
			element.disabled = 1;
			element =
				timlib_get_element(
					"from_exceedance_format_yn_0" );
			element.disabled = 1;
		}
	}

	return true;
} // post_change_annual_periods()

