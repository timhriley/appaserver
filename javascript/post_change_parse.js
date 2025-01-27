//
// post_change_parse.js
// ---------------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function post_change_parse( row )
{
	var position_element_name;
	var position_element;

	position_element_name = 'position_' + row;
	position_element = timlib_get_element( position_element_name );

	if ( position_element.value < 5 )
	{
		alert( 'Error: position must be at least 5.' );
	}
	return true;
} // post_change_parse()

