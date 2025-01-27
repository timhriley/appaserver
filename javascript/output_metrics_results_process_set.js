//
// output_metrics_results_process_set.js
// -------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_output_metrics_results( current_element )
{
	var process_element;
	var element;
	var process_value;
	var aggregate_level_value;

	element =
		timlib_get_element(
			"units|units_converted_0" );
	element.disabled = 0;
	element =
		timlib_get_element(
			"from_email_address_0" );
	element.disabled = 0;

	process_element = timlib_get_element( "output_metrics_results_0" );

	process_value =
		timlib_get_drop_down_element_value(
			process_element.options );

	if ( process_value == 'output_metrics_results_exceedance' )
	{
		element =
			timlib_get_element(
				"from_email_address_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_metrics_results_exceedance_table' )
	{
		element =
			timlib_get_element(
				"from_email_address_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_metrics_results_easychart' )
	{
		element =
			timlib_get_element(
				"from_email_address_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_metrics_results_gracechart' )
	{
		element =
			timlib_get_element(
				"from_email_address_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_metrics_results_statistics' )
	{
		element =
			timlib_get_element(
				"aggregate_level_0" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"aggregate_statistic_0" );
		element.disabled = 1;
		element =
			timlib_get_element(
				"from_email_address_0" );
		element.disabled = 1;
	}
	else
	if ( process_value == 'output_metrics_results_text_file' )
	{
	}

	return true;
} // post_change_output_metrics_results()

