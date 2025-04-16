// $APPASERVER_HOME/src_predictive/post_change_insert_expense_transaction.js
// -------------------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// -------------------------------------------------------------------------
function post_change_insert_expense_transaction()
{
	var element;
	var value;

	// Turn on edits
	// -------------
	element = document.getElementById( "full_name^street_address" );
	element.disabled = 0;

	element = document.getElementById( "full_name" );
	element.disabled = 0;

	// Make mutually exclusive
	// -----------------------
	element = document.getElementById( "full_name^street_address" );
	value = timlib_drop_down_element_value( element.options );

	if ( value != 'select' )
	{
		element = document.getElementById( "full_name" );
		element.disabled = 1;

		return true;
	}

	element = document.getElementById( "full_name" );

	if ( element.value != "" )
	{
		element = document.getElementById( "full_name^street_address" );
		element.disabled = 1;

		return true;
	}

	return true;
}
