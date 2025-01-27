// output_daily_moving_average.js
// ------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_output_daily_moving_average( current_element )
{
	var aggregate_statistic_element;
	var aggregate_statistic_value;

	aggregate_statistic_element =
		timlib_get_element( "aggregate_statistic_0" );

	aggregate_statistic_value =
		timlib_get_drop_down_element_value(
			aggregate_statistic_element.options );

//	if ( aggregate_statistic_value != undefined
//	&&   aggregate_statistic_value != 'undefined'
//	&&   aggregate_statistic_value != 'select' )
//	{
//		if ( aggregate_statistic_value != 'minimum'
//		&&   aggregate_statistic_value != 'average'
//		&&   aggregate_statistic_value != 'maximum'
//		&&   aggregate_statistic_value != 'sum'
//		&&   aggregate_statistic_value != 'median' )
//		{
//			alert( 'Warning: Aggregate Statistic is limited to minimum, average, maximum, median, or sum. Resetting.' );
//			timlib_set_drop_down_element_value(
//				aggregate_statistic_element.options,
//				'select' );
//		}
//	}

	return true;
}

