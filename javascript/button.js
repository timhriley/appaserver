// $APPASERVER_HOME/javascript/button.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function button_multi_select_all(
	original_element_name,
	element_name )
{
	let original_element =
		document.getElementById(
			original_element_name );

	if ( !original_element ) return false;

	for ( const option of original_element.options )
		option.selected = false;

	let element =
		document.getElementById(
			element_name );

	if ( !element ) return false;

	for ( const option of element.options )
	{
		if ( !button_empty_option_boolean( option ) )
			option.selected = true;
	}
}

function button_multi_move_right(
	original_element_name,
	element_name,
	recall_multi_index_delimiter )
{
	let original_element =
		document.getElementById(
			original_element_name );
	
	let element =
		document.getElementById(
			element_name );
	
	for ( var i = 0; i < original_element.options.length; i++ )
	{
		if (	original_element.options[ i ].selected
		&&	!button_empty_option_boolean(
				original_element.options[ i ] ) )
		{
			button_multi_option_move_right(
				recall_multi_index_delimiter,
				original_element.options[ i ].value,
				original_element.options[ i ].text,
				element.options
					/* element_option_array */
					/* Set new option */,
				i /* index */ );

			original_element.options[ i ] =
				button_empty_option();
		}
	}
}

function button_multi_option_move_right(
	recall_multi_index_delimiter,
	option_value,
	option_text,
	element_option_array
		/* Sets new option */,
	index )
{
	let value_with_index =
		option_value +
		recall_multi_index_delimiter +
		index;
	
	for ( i = 0; i < element_option_array.length; i++ )
	{
		if ( button_empty_option_boolean(
			element_option_array[ i ] ) )
		{
			break;
		}
	}
	
	element_option_array[ i ] =
		new Option(
			option_text,
			value_with_index,
			false /* defaultSelected */,
			true /* selected */ );

	return true;
}

function button_multi_move_left(
	original_element_name,
	element_name,
	recall_multi_index_delimiter )
{
	let original_element =
		document.getElementById(
			original_element_name );

	let element =
		document.getElementById(
			element_name );

	for ( var i = 0; i < element.options.length; i++ )
	{
		if ( element.options[ i ].selected )
		{
			let value_with_index = element.options[ i ].value;

			let value_array =
				value_with_index.split(
					recall_multi_index_delimiter );

			original_element.
				options[ value_array[ 1 ] ] =
					new Option(
						element.options[ i ].text,
						value_array[ 0 ],
						false /* not defaultSelected */,
						false /* not selected */ );

			element.options[ i ] = button_empty_option();
		}
	}

	return true;
}

function button_reset(
	form,
	recall_multi_index_delimiter )
{
	for ( const element of form.elements )
	{
		if (	element.type == "select-one"
		||	element.type == "select-multiple"
		||	element.type == "text" )
		{
			element.disabled = false;
		}

		// The single selects don't reset on Konqueror.
		// --------------------------------------------
		if ( element.type == "select-one" )
		{
			let option = element.options[ element.selectedIndex ];
			if ( option.value != "select" ) option.selected = false;
		}

		if (	element.type == "select-multiple"
		&&	element.name.substring( 0, 9 ) == "original_" )
		{
			let element_name = element.name.substring( 9 );

			button_multi_select_all(
				element.name /* original_element_name */,
				element_name );

			button_multi_move_left(
				element.name /* original_element_name */,
				element_name,
				recall_multi_index_delimiter );
		}
	}

	form.reset();

	return true;
}

function button_empty_option()
{
	let option =
	new Option(
		"" /* text */,
		"" /* value */,
		false /* defaultSelected */,
		true /* selected (Prevent list from scrolling to the top) */ );

	return option;
}

function button_empty_option_boolean(
	option )
{
	if ( option.value == "" )
		return true;
	else
		return false;
}
