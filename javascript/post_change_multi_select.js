// $APPASERVER_HOME/javascript/post_change_multi_select.js
// -------------------------------------------------------------
// No warranty and freely available software. See appaserver.org
// -------------------------------------------------------------

function post_change_multi_select_all( element_name )
{
	var element;
	var select_element_name;
	var original_element_name;
	var value;
	var i;

	// Select those positions that are populated
	// -----------------------------------------
	select_element_name = element_name;
	element = timlib_get_element( select_element_name )

	if ( !element )
	{
		alert( 'ERROR: cannot get element = ' + element_name );
		return false;
	}

	for( i = 0; i < element.options.length; i++ )
	{
		value = element.options[ i ].value;

		if ( value != "" )
		{
			element.options[ i ].selected = 1;
		}
	}

	// Unselect everything populated in the original
	// ---------------------------------------------
	original_element_name = 'original_' + element_name;
	element = timlib_get_element( original_element_name )

	if ( element )
	{
		for( i = 0; i < element.options.length; i++ )
		{
			value = element.options[ i ].value;

			if ( value != "" )
			{
				element.options[ i ].selected = 0;
			}
		}
	}

	return true;
}

function post_change_multi_select_move_right(
		original_element_name, to_element_name, index_delimiter )
{
	var original_element;
	var to_element;
	var i;
	var j;
	var new_text;
	var new_value;
	var new_value_with_index;

	original_element = timlib_get_element( original_element_name )
	to_element = timlib_get_element( to_element_name )

	for( i = 0; i < original_element.options.length; i++ )
	{
		if ( original_element.options[ i ].selected )
		{
			new_value = original_element.options[ i ].value;

			if ( new_value != "" )
			{
				new_value_with_index =
					new_value + index_delimiter + i;

				new_text = original_element.options[ i ].text;
	
				for(	j = 0;
					j < to_element.options.length;
					j++ )
				{
					if ( to_element.options[ j ].text=="" )
					{
						break;
					}
				}

				to_element.options[ j ] =
					new Option(
						new_text,
						new_value_with_index,
						0,
						1 /* is selected */ );

				// Set the selected flag to prevent the
				// list from scrolling to the top.
				// ------------------------------------
				original_element.options[ i ] =
					new Option( "", "", 0, 1 );
			}
		}
	}
	return true;
}

function post_change_multi_select_move_left(
		original_element_name, to_element_name, index_delimiter )
{
	var original_element;
	var to_element;
	var i;
	var option_index;
	var new_text;
	var new_value;
	var value_with_index;
	var value_array;

	original_element = timlib_get_element( original_element_name )

	if ( !original_element )
	{
		// Sometimes it begins with "from_"
		// --------------------------------
		original_element_name = "from_" + original_element_name;
		original_element = timlib_get_element( original_element_name )
	}

	to_element = timlib_get_element( to_element_name )

	if ( !to_element )
	{
		// Sometimes it begins with "from_"
		// --------------------------------
		to_element_name = "from_" + to_element_name;
		to_element = timlib_get_element( to_element_name )
	}

	for( i = 0; i < to_element.options.length; i++ )
	{
		if ( to_element.options[ i ].selected )
		{
			value_with_index = to_element.options[ i ].value;

			if ( value_with_index != "" )
			{
				value_array =
					value_with_index.split(
						index_delimiter );

				new_text = to_element.options[ i ].text;
	
				original_element.
					options[ value_array[ 1 ] ] =
					new Option(
						new_text,
						value_array[ 0 ],
						0,
						0 /* not selected */ );
	
				to_element.options[ i ] =
					new Option( "", "", 0, 0 );
			}
		}
	}
	return true;
}

