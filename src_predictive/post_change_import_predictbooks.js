// $APPASERVER_HOME/src_predictive/post_change_import_predictbooks.js
// -----------------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// -----------------------------------------------------------------------
function post_change_import_predictbooks()
{
	var bank_of_america_element;
	var bank_full_name_element;

	bank_of_america_element = timlib_element( "bank_of_america_yn" );
	bank_of_america_element.disabled = 0;

	bank_full_name_element = timlib_element( "bank_full_name" );
	bank_full_name_element.disabled = 0;

	if ( bank_of_america_element.value == 'y' )
	{
		bank_full_name_element.disabled = 1;
	}
	else
	if ( bank_full_name_element.value != '' )
	{
		bank_of_america_element.disabled = 1;
	}

	return true;
}

