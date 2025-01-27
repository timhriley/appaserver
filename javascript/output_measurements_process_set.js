// output_measurements_process_set.js
// ---------------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_output_measurements( current_element )
{
	var process_element;
	var element;
	var process_value;
	var aggregate_level_value;
	var value;

	// Check if preprompt screen
	// -------------------------
	element =
		timlib_get_element(
			"original_datatype_1" );

	if ( element )
	{
		post_change_output_measurements_preprompt();
		return true;
	}

	element =
		timlib_get_element(
			"zulu_time_yn" );
	element.disabled = 1;
	element =
		timlib_get_element(
			"aggregate_level" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"aggregate_statistic" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"validation_level" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"units^units_converted" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"email_address" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"transmit_validation_info_yn" );
	element.disabled = 1;
	element =
		timlib_get_element(
			"days_to_average" );
	element.disabled = 1;
	element =
		timlib_get_element(
			"accumulate_yn" );
	element.disabled = 1;

	process_element = timlib_get_element( "output_measurements" );

	process_value =
		timlib_get_drop_down_element_value(
			process_element.options );

	if ( process_value == 'output_measurement_spreadsheet' )
	{
		element =
			timlib_get_element(
				"zulu_time_yn" );
		element.disabled = 0;
	}
	else
	if ( process_value == 'chart_high_average_low' )
	{
		element =
			timlib_get_element(
				"email_address" );
		element.disabled = 1;

		element =
			timlib_get_element(
				"aggregate_level" );
		element.disabled = 0;

		element =
			timlib_get_element(
				"aggregate_statistic" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_daily_moving_statistic' )
	{
		element =
			timlib_get_element(
				"email_address" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_level" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"days_to_average" );
		element.disabled = 0;

	}
	else
	if ( process_value == 'output_measurement_googlecharts' )
	{
		element =
			timlib_get_element(
				"email_address" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"accumulate_yn" );
		element.disabled = 0;
	}
	else
	if ( process_value == 'easychart_measurements' )
	{
		element =
			timlib_get_element(
				"email_address" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"accumulate_yn" );
		element.disabled = 0;
	}
	else
	if ( process_value == 'chart_measurements' )
	{
		element =
			timlib_get_element(
				"accumulate_yn" );
		element.disabled = 0;
	}
	else
	if ( process_value == 'output_measurement_table' )
	{
		element =
			timlib_get_element(
				"accumulate_yn" );
		element.disabled = 0;

		element =
			timlib_get_element(
				"zulu_time_yn" );
		element.disabled = 0;
	}
	else
	if ( process_value == 'statistics_report' )
	{
		element =
			timlib_get_element(
				"aggregate_level" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_statistic" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"email_address" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'text_file'
	||   process_value == 'transmit_measurements' )
	{
		element =
			timlib_get_element(
				"transmit_validation_info_yn" );
		element.disabled = 0;
		element =
			timlib_get_element(
				"accumulate_yn" );
		element.disabled = 0;

		element =
			timlib_get_element(
				"zulu_time_yn" );
		element.disabled = 0;
	}
	else
	if ( process_value == 'quick_measurement_form' )
	{
		element =
			timlib_get_element(
				"aggregate_level" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_statistic0" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"email_address" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"validation_level" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"units^units_converted" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'data_validation_form' )
	{
		element =
			timlib_get_element(
				"aggregate_level" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_statistic" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"email_address" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"validation_level" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"units^units_converted" );
		element.disabled = 1;
	}

	element =
		timlib_get_element(
			"aggregate_level" );

	aggregate_level_value =
		timlib_get_drop_down_element_value(
			element.options );

	if ( aggregate_level_value != undefined
	&&   aggregate_level_value != 'undefined'
	&&   aggregate_level_value != 'select'
	&&   aggregate_level_value != 'real_time' )
	{
		element =
			timlib_get_element(
				"zulu_time_yn" );
		element.disabled = 1;
	}

	if ( ( aggregate_level_value == undefined
	||   aggregate_level_value == 'undefined'
	||   aggregate_level_value == 'select'
	||   aggregate_level_value == 'real_time' )
	&&   process_value != 'output_daily_moving_statistic' )
	{
		element =
			timlib_get_element(
				"aggregate_statistic" );
		element.disabled = 1;
	}

	return true;

} // post_change_output_measurements()

function post_change_output_measurements_preprompt()
{
	var element;
	var station_populated;
	var favorite_station_populated;

	// Set defaults
	// ------------
	element =
		timlib_get_element(
			"original_station_1" );
	element.disabled = false;

	element =
		timlib_get_element(
			"station_1" );
	element.disabled = false;

	element =
		timlib_get_element(
			"favorite_station_set_name^login_name_0" );
	element.disabled = false;

	// Set station_populated
	// ---------------------
	element =
		timlib_get_element(
			"station_1" );

	station_populated =
		timlib_get_multi_select_drop_down_element_populated(
			element.options );

	// Set favorite_station_populated
	// ------------------------------
	element =
		timlib_get_element(
			"favorite_station_set_name^login_name_0" );

	favorite_station_populated =
		timlib_get_drop_down_element_populated(
			element.options );


	// Set disabled
	// ------------
	if ( station_populated )
	{
		element =
			timlib_get_element(
				"favorite_station_set_name^login_name_0" );
		element.disabled = true;
	}
	else
	if ( favorite_station_populated )
	{
		element =
			timlib_get_element(
				"original_station_1" );
		element.disabled = true;

		element =
			timlib_get_element(
				"station_1" );
		element.disabled = true;

	}

} // post_change_output_measurements_preprompt()

