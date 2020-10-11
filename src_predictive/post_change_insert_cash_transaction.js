// src_predictive/post_change_insert_cash_transaction.js
// -----------------------------------------------------

function post_change_insert_cash_transaction( current_element )
{
	var element;
	var value;

	// Turn on edits
	// -------------
	element = timlib_get_element( "full_name^street_address_0" );
	element.disabled = 0;

	element = timlib_get_element( "from_full_name_0" );
	element.disabled = 0;

	element = timlib_get_element( "from_street_address_0" );
	element.disabled = 0;

	// Make mutually exclusive
	// -----------------------
	element = timlib_get_element( "full_name^street_address_0" );
	value = timlib_get_drop_down_element_value( element.options );

	if ( value != 'select' )
	{
		element = timlib_get_element( "from_full_name_0" );
		element.disabled = 1;

		element = timlib_get_element( "from_street_address_0" );
		element.disabled = 1;

		return true;
	}

	element = timlib_get_element( "from_full_name_0" );

	if ( element.value != "" )
	{
		element = timlib_get_element( "full_name^street_address_0" );
		element.disabled = 1;

		return true;
	}

	return true;
}

