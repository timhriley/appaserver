// $APPASERVER_HOME/javascript/post_change_attribute.js
// ----------------------------------------------------
function post_change_attribute( row )
{
	var element_name;
	var element;
	var attribute_datatype_value;

	if ( row == 0 )
	{
		post_change_attribute_each_row();
		// return true; <-- Need to pass through for insert screen.
	}

	// Turn off edit
	// -------------
	element_name = 'float_decimal_places_' + row;
	element = timlib_get_element( element_name );

	if ( element == "" ) return false;

	element.disabled = true;

	// Turn on (maybe)
	// ---------------
	element_name = 'attribute_datatype_' + row;
	element = timlib_get_element( element_name );

	attribute_datatype_value =
		timlib_get_drop_down_element_value(
			element.options );

	if ( attribute_datatype_value == "float" )
	{
		element_name = 'float_decimal_places_' + row;
		element = timlib_get_element( element_name );

		if ( element == "" ) return false;

		element.disabled = false;
	}

	return true;
}

function post_change_attribute_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = post_change_attribute( row );
		if ( results == false ) break;
		row++;
	}

	return true;

}
