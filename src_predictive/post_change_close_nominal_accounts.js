// $APPASERVER_HOME/src_predictive/post_change_close_nominal_accounts.js
// ---------------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------------
function post_change_close_nominal_accounts()
{
	var element;
	var value;

	element = timlib_element( "as_of_date" );
	element.disabled = 0;

	element = timlib_element( "undo_yn" );
	value = timlib_drop_down_element_value( element.options );

	if ( value == 'y' )
	{
		element = timlib_element( "as_of_date" );
		element.disabled = 1;
	}

	return true;
}

