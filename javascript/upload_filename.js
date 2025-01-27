// upload_filename.js
// ---------------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function upload_filename_replace_slash( file_element )
{
	var value;
	var new_value;
	var len;
	var i;
	var c;

	value = file_element.value;
	new_value = new String( '' );

	len = value.length;
	if ( !len ) return '';

	for ( i = 0; i < len; i++ )
	{
		c = value.charAt( i );
		if ( c == '/' ) c = '\\';
		new_value = new_value.concat( c );
	}
	return new_value;
}

function upload_filename_check( form, element_name_list_string, delimiter )
{
	var element_name;
	var element_name_list;
	var new_value;
	
	element_name_list =
		element_name_list_string.split( delimiter );

	for ( var element in form.elements )
	{
		element_name = element.toString();

		if ( timlib_exists( element_name, element_name_list ) )
		{
			if ( form.elements[ element ] == undefined )
				continue;

			value = form.elements[ element ].value;

			if ( ( value != undefined )
			&&   ( value != 'undefined' )
			&&   ( value != 'select' )
			&&   ( value != '' ) )
			{
				if ( form.elements[ element ].type == 'file' )
				{
					new_value =
					upload_filename_replace_slash(
						form.elements[ element ] );
					form.elements[ element ] = new_value;
				}
			}
		}
	}
}

