// $APPASERVER_HOME/src_predictive/post_change_investment_account.js
// -----------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// -----------------------------------------------------------------
function post_change_investment_account( row )
{
	var element_name;
	var element;
	var element_value;

	if ( row == 0 )
	{
		post_change_investment_account_each_row();
		// return true; <-- Need to pass through for insert screen.
	}

	// Turn on each edit.
	// ------------------
	element_name = 'certificate_maturity_months_' + row;
	element = timlib_get_element( element_name );

	// All done
	// --------
	if ( element == "" ) return false;

	element.disabled = false;

	element_name = 'certificate_maturity_date_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	// Turn off if not certificate
	// ---------------------------
	element_name = 'investment_classification_' + row;

	element = timlib_get_element( element_name );

	element_value =
		// ---------------------------------
		// Returns 'undefined' if not found.
		// ---------------------------------
		timlib_get_drop_down_element_value(
			element.options );

	if ( element_value == 'undefined' )
	{
		alert( 'ERROR: value undefined for Investment Classification' );
		return false;
	}

	if ( element_value != 'certificate' )
	{
		element_name = 'certificate_maturity_months_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'certificate_maturity_date_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}

	return true;
}

function post_change_investment_account_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = post_change_investment_account( row );
		if ( results == false ) break;
		row++;
	}

	return true;
}
