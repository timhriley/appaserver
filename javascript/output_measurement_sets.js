// output_measurements_set.js
// --------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_output_measurement_sets( current_element )
{
	var process_element;
	var element;
	var process_value;

	process_element = timlib_get_element( "output_measurement_sets_0");

	process_value =
		timlib_get_drop_down_element_value(
			process_element.options );

	element =
		timlib_get_element(
			"from_plot_for_station_check_yn_0" );
	element.disabled = 0;

	if ( process_value == 'data_sets_transmit'
	||   process_value == 'data_sets_table'
	||   process_value == 'data_sets_googlechart' )
	{
		element.disabled = 1;
	}

	return true;
} // post_change_output_measurement_sets()

