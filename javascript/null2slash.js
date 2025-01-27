// $APPASERVER_HOME/javascript/null2slash.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function null2slash( element )
{
	var regular_expression = "password_[0-9][0-9]*$";
	var results;

	if ( left_trim( element.value ) == '' ) element.value = '/';

	results = element.name.match( regular_expression );

	if ( results != null && element.value == '/' )
	{
		alert( 'Warning: this account will be locked.' );
	}

	return true;
}

