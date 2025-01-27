//
// water_budget_flow.js
// --------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_water_budget_flow( current_element )
{
	var merged_output_element;
	var merged_output_value;
	var display_count_element;
	var aggregate_level_element;
	var aggregate_level_value;

	aggregate_level_element =
		timlib_get_element( "aggregate_level_0" );

	aggregate_level_value =
		timlib_get_drop_down_element_value(
			aggregate_level_element.options );

	display_count_element =
		timlib_get_element(
			"from_display_count_yn_0" );
	display_count_element.disabled = 1;

	merged_output_element =
		timlib_get_element( "merged_output_0" );

	merged_output_value =
		timlib_get_drop_down_element_value(
			merged_output_element.options );

	if ( aggregate_level_value != undefined
	&&   aggregate_level_value != 'undefined'
	&&   aggregate_level_value != 'real_time'
	&&   aggregate_level_value != 'select' )
	{
		if ( merged_output_value == 'table'
		||   merged_output_value == 'transmit' )
		{
			display_count_element.disabled = 0;
		}
	}

	return true;
}

