// $APPASERVER_HOME/src_predictive/post_change_pay_liabilities.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------
function post_change_pay_liabilities( current_element )
{
	var element;
	var value;

	element = timlib_element( "account" );
	element.disabled = 0;

	element = timlib_element( "starting_check_number" );
	element.disabled = 0;

	element = timlib_element( "memo" );
	element.disabled = 1;

	// If starting_check_number is set
	// then no account and yes memo.
	// --------------------------------
	element = timlib_element( "starting_check_number" );
	value = element.value;

	if ( value >= 1 )
	{
		element = timlib_element( "memo" );
		element.disabled = 0;

		element = timlib_element( "account" );
		element.disabled = 1;

		return true;
	}

	// If account is set
	// then no starting_check_number.
	// ------------------------------
	element = timlib_element( "account" );

	if ( timlib_drop_down_element_populated( element.options ) )
	{
		element = timlib_element( "starting_check_number" );
		element.disabled = 1;

		return true;
	}

	return true;
}

