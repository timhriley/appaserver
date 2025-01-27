// -----------------------------------------------------------
// $APPASERVER_HOME/javascript/post_change_folder_attribute.js
// -----------------------------------------------------------
function post_change_folder_attribute( row )
{
	var element;
	var primary_key_element_value;
	var display_order_element_value;

	if ( row == -1 )
	{
		post_change_folder_attribute_each_row();
		return true;
	}

	// Turn on each edit.
	// ------------------
	element = timlib_row_element( "primary_key_index", row );
	if ( element == false ) return false;
	element.disabled = false;

	element = timlib_row_element( "display_order", row );
	if ( element == false ) return false;
	element.disabled = false;

	element = timlib_row_element( "primary_key_index", row );
	if ( element.value == "/" ) return true;
	primary_key_element_value = Number( element.value );

	element = timlib_row_element( "display_order", row );
	if ( element.value == "/" ) return true;
	display_order_element_value = Number( element.value );

	// Turn off one of them
	// --------------------
	if ( primary_key_element_value )
	{
		element = timlib_row_element( "display_order", row );
		element.disabled = true;
	}
	else
	if ( display_order_element_value )
	{
		element = timlib_row_element( "primary_key_index", row );
		element.disabled = true;
	}

	return true;
}

function post_change_folder_attribute_each_row()
{
	var row = 1;
	var result;

	while( true )
	{
		result = post_change_folder_attribute( row );
		if ( result == false ) break;
		row++;
	}

	return true;
}
