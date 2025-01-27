//
// merged_datasets_process.js
// --------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_merged_datasets( current_element )
{
	var output_medium_element;
	var output_medium_value;
	var display_count_element;
	var aggregate_level_element;
	var aggregate_level_value;
	var omit_output_if_any_missing_yn_element;

	aggregate_level_element =
		timlib_get_element( "aggregate_level_0" );

	aggregate_level_value =
		timlib_get_drop_down_element_value(
			aggregate_level_element.options );

	display_count_element =
		timlib_get_element(
			"from_display_count_yn_0" );
	display_count_element.disabled = 1;

	output_medium_element =
		timlib_get_element( "output_medium_0" );

	output_medium_value =
		timlib_get_drop_down_element_value(
			output_medium_element.options );

	omit_output_if_any_missing_yn_element =
		timlib_get_element( "omit_output_if_any_missing_yn_0" );

	omit_output_if_any_missing_yn_element.disabled = 0;

	if ( aggregate_level_value != undefined
	&&   aggregate_level_value != 'undefined'
	&&   aggregate_level_value != 'real_time'
	&&   aggregate_level_value != 'select' )
	{
		if ( output_medium_value == 'table' )
		{
			display_count_element.disabled = 0;
		}
	}

	if ( output_medium_value == 'gracechart' )
	{
		omit_output_if_any_missing_yn_element.disabled = 1;
	}

	return true;

} // post_change_merged_datasets()

