// ------------------------------------------------------
// $APPASERVER_HOME/javascript/verify_attribute_widths.js
// ------------------------------------------------------

function verify_attribute_widths(
		element_name_list_string,
		element_attribute_width_list_string,
		source_form )
{
	var element;
	var element_value;
	var element_name;
	var element_name_list;
	var element_attribute_width_list;
	var element_attribute_width;
	var i;
	var element_value_length;
	var return_value;

	return_value = true;

	if ( source_form == "output_prompt_edit_form" )
	{
		element_value =
			timlib_get_radio_selected_value(
				"lookup_option_radio_button" );

		if ( element_value != "new_button" )
		{
			return true;
		}
	}

	element_name_list =
		element_name_list_string.split( ',' );

	element_attribute_width_list =
		element_attribute_width_list_string.split( ',' );

	for( i = 0; i < element_name_list.length; i++ )
	{
		element_name = element_name_list[ i ];
		element_attribute_width = element_attribute_width_list[ i ];
		element = timlib_get_element( element_name );

		element_value_length = element.value.length;

		if ( element_value_length > element_attribute_width )
		{
			alert(
'Error: ' + element_name + ' has size of ' + element_value_length + '. This exceeds the maximum size of ' + element_attribute_width + ' by ' + (element_value_length - element_attribute_width) + '.'
			);

			return_value = false;
		}
	}
	return return_value;
}

