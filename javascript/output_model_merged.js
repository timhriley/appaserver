//
// output_model_merged.js
// ----------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_output_model_merged( current_element )
{
	var aggregate_level_element;
	var aggregate_level_value;

	aggregate_level_element =
		timlib_get_element( "aggregate_level_0" );

	aggregate_level_value =
		timlib_get_drop_down_element_value(
			aggregate_level_element.options );

	if ( aggregate_level_value == 'hourly' )
	{
		if ( current_element.name == 'aggregate_level_0' )
		{
			alert( 'Warning: This aggregate level is not appropriate for model results. Resetting.' );
		}

		timlib_set_drop_down_element_value(
			aggregate_level_element.options,
			'Select' );
	}

	return true;
} // post_change_output_model_merged()

