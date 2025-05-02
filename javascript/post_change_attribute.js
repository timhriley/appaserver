// ----------------------------------------------------
// $APPASERVER_HOME/javascript/post_change_attribute.js
// ----------------------------------------------------
function post_change_attribute( row )
{
	var element;
	var attribute_datatype_value;

	// Negative row for body onload.
	// -----------------------------
	if ( row == -1 )
	{
		post_change_attribute_each_row();
		return true;
	}

	// Turn off edit
	// -------------
	element = timlib_row_element( "float_decimal_places", row );

	if ( element == false ) return false;

	element.disabled = true;

	// Turn on edit (maybe)
	// --------------------
	element = timlib_row_element( "column_datatype", row );

	// If on edit frame, but prompt frame ignored column_datatype.
	// -----------------------------------------------------------
	if ( element == false ) return false

	attribute_datatype_value =
		timlib_drop_down_element_value(
			element.options );

	if ( attribute_datatype_value == "float" )
	{
		element = timlib_row_element( "float_decimal_places", row );
		element.disabled = false;
	}

	return true;
}

function post_change_attribute_each_row()
{
	var row = 1;
	var result;

	while( true )
	{
		result = post_change_attribute( row );
		if ( result == false ) break;
		row++;
	}

	return true;
}
