// exceedance_curve_process.js
// ---------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_exceedance_curve( current_element )
{
	var chart_yn_element;
	var chart_yn_value;
	var email_address_element;

	chart_yn_element =
		timlib_get_element( "from_chart_yn_0" );

	email_address_element =
		timlib_get_element( "from_email_address_0" );
	email_address_element.disabled = 0;

	chart_yn_value =
		timlib_get_drop_down_element_value(
			chart_yn_element.options );

	if ( chart_yn_value == 'n' )
	{
		email_address_element.disabled = 1;
	}

	return true;
} // post_change_exceedance_curve()

