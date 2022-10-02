// src_predictive/post_change_close_nominal_accounts.js
// ----------------------------------------------------

function post_change_close_nominal_accounts( current_element )
{
	var element;
	var value;

	element = timlib_get_element( "from_as_of_date_0" );
	element.disabled = 0;

	element = timlib_get_element( "from_undo_yn_0" );
	value = timlib_get_drop_down_element_value( element.options );

	if ( value == 'y' )
	{
		element = timlib_get_element( "from_as_of_date_0" );
		element.disabled = 1;
	}

	return true;
}

