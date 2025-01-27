//
// src_hydrology/post_change_datatype.js
// -------------------------------------

function post_change_datatype( element )
{
	var value;
	var attribute_name;

	attribute_name = timlib_remove_index( element.name );

	if ( attribute_name == 'datatype' )
	{
		value = element.value;
		var len;
		var i;
		var ch;

		len = value.length;

		if ( !len ) return true;

		for ( i = len - 1; i; i-- )
		{
			ch = value.charAt( i );

			if ( ch == ' ' )
			{
				alert(
				'Error: datatype cannot contain spaces.' );
				element.value = "";
				return false;
			}
		}
	}

	return true;

} // post_change_datatype()

