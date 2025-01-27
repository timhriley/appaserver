// $APPASERVER_HOME/javascript/table_edit.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function table_edit_reset( form, index_delimiter )
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
				timlib_drop_down_element_value(
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
}

function table_edit_element( search_element_name, form_number )
{
	var element;
	var i;
	var form;

	form = document.forms[ form_number ];

	for( i = 0; i < form.elements.length; i++ )
	{
		element = form.elements[ i ];

		if ( element.name == search_element_name )
		{
			return element;
		}
	}

	return "";
}

var delete_warned_already = false;

function table_edit_delete_button_warning()
{
	if ( !delete_warned_already )
	{
		alert( "Warning: you pressed the delete button." );
	}

	delete_warned_already = true;

	return true;
}

function table_edit_push_button_set_all(
			operation_name,
			form_number )
{
	var set_all_element;
	var push_button_element_name;
	var push_button_element;
	var row;

	set_all_element =
		table_edit_element(
			operation_name, form_number );

	for( row = 1; ; row++ )
	{
		push_button_element_name = operation_name + "_" + row;

		push_button_element =
			table_edit_element(
				push_button_element_name,
				form_number );

		if ( !push_button_element ) break;

		push_button_element.checked = set_all_element.checked;
	}

	return 1;
}

