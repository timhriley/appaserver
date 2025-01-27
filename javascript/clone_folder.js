// $APPASERVER_HOME/javascript/clone_folder.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function post_change_clone_folder( current_element )
{
	var folder_element;
	var folder_element_value;
	var folder_attribute_element;
	var folder_attribute_element_value;

	folder_element = timlib_get_element( "folder_0");
	folder_element.disabled = 0;

	folder_attribute_element = timlib_get_element( "folder|attribute_0");
	folder_attribute_element.disabled = 0;

	folder_element_value =
		timlib_get_drop_down_element_value(
			folder_element.options );

	folder_attribute_element_value =
		timlib_get_drop_down_element_value(
			folder_attribute_element.options );

	if ( folder_element_value != 'select' )
		folder_attribute_element.disabled = 1;
	else
	if ( folder_attribute_element_value != 'select' )
		folder_element.disabled = 1;

	return true;
} // post_change_clone_folder()

