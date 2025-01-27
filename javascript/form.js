// $APPASERVER_HOME/javascript/form.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function form_reset( form, index_delimiter )
{
	var i;
	var to_element_name;
	var original_element_name;
	var element;
	var value;

	for( i = 0; i < form.elements.length; i++ )
	{
		element = form.elements[ i ];

		if ( element.type == 'select-one'
		||   element.type == 'select-multiple'
		||   element.type == 'text' )
		{
			element.disabled = 0;
		}

		// ---------------------------------------------
		// Find the single selects.
		// Note: form.reset() doesn't work on Konqueror.
		// ---------------------------------------------
		if ( element.type == 'select-one' )
		{
			value =
				timlib_get_drop_down_element_value(
					element.options );

			if ( value != 'select' )
			{
				timlib_set_drop_down_element_value(
					element.options,
					'select' );
			}
		}

		// Find the multi-selects
		// ----------------------
		original_element_name = element.name;
		if ( element.type == 'select-multiple'
		&&   original_element_name.substring( 0, 9 ) == "original_" )
		{
			to_element_name = original_element_name.substring( 9 );
			post_change_multi_select_all( to_element_name );
			post_change_multi_select_move_left(
				original_element_name,
				to_element_name,
				index_delimiter );
		}
	}

	form.reset();
	return true;
}

function form_name_element(
		form_name,
		element_name )
{
	var form_element;
	var element;

	form_element = document.getElementById( form_name );

	if ( !form_element )
	{
		return false;
	}

	element = form_element.elements[ element_name ];

	if ( !element )
		return false;
	else
		return element;
}

function form_push_button_set_all(
		form_name,
		operation_name )
{
	var set_all_element;
	var push_button_element_name;
	var push_button_element;
	var row;

	set_all_element =
		form_name_element(
			form_name,
			operation_name );

	if ( !set_all_element )
	{
		alert( 'form_name_element() returned false' );
		return false;
	}

	for( row = 1; ; row++ )
	{
		push_button_element_name = operation_name + "_" + row;

		push_button_element =
			form_name_element(
				form_name,
				push_button_element_name );

		if ( !push_button_element ) break;

		push_button_element.checked = set_all_element.checked;
	}

	return true;
}

