// frame.js
// -----------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function frame_initialize(	frame_name,
				element_name_string_array_string,
				option_value_string_array_string,
				option_display_string_array_string )
{
	var element_name_string_array;
	var element_name_string;
	var element_option_value_string_array;
	var element_option_display_string_array;
	var option_value_array;
	var option_display_array;
	var i, length;
	var my_dynamic_drop_down_array;

	my_dynamic_drop_down_array = new dynamic_drop_down_array()

	// Parse element name
	// ------------------
	element_name_string_array =
		element_name_string_array_string.split( "^" );

	length = element_name_string_array.length;

	// Parse element_option_value_string_array
	// ---------------------------------------
	element_option_value_string_array =
		option_value_string_array_string.split( "^" );

	// Parse element_option_display_string_array
	// -----------------------------------------
	element_option_display_string_array =
		option_display_string_array_string.split( "^" );

	for( i = 0; i < length; i++ )
	{
		element_name_string = element_name_string_array[ i ];

		option_value_array = 
			element_option_value_string_array[ i ].split( "|" );

		option_display_array = 
			element_option_display_string_array[ i ].split( "|" );

		my_dynamic_drop_down_array.set_element(
					element_name_string,
					option_value_array,
					option_display_array );
	}

	frame_populate_drop_down_elements( 	frame_name,
						my_dynamic_drop_down_array );

} // frame_initialize()

function frame_populate_drop_down_elements( 	frame_name,
						my_dynamic_drop_down_array )
{
	var i;
	var element;
	var length;
	var frame_index;
	var element_offset;

	frame_index = frame_name2index( frame_name );

	if ( frame_index == -1 )
	{
		alert( "Cannot find frame name = " + frame_name );
		return;
	}

	length = 
	parent.frames[ frame_index ].document.forms[ 0 ].elements.length;

	for ( i = 0; i < length; i++ )
	{
		element = 
		parent.frames[ frame_index ].document.forms[ 0 ].elements[ i ];

		element_offset = 
		my_dynamic_drop_down_array.exists_element( element.name );

		if ( element_offset >= 0 )
		{
			my_dynamic_drop_down_array.populate( 
							element, 
							element_offset );
		}
	}

} // function frame_populate_drop_down_elements()



function frame_name2index( frame_name )
{
	var length;
	var name;

	length = parent.frames.length;
	for ( i = 0; i < length; i++ )
	{
		name = parent.frames[ i ].name;
		if ( name == frame_name ) return i;
	}
	return -1;
} // frame_name2index()
