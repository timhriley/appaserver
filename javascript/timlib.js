// $APPASERVER_HOME/javascript/timlib.js
// -------------------------------------------------------------
// No warranty and freely available software. See appaserver.org
// -------------------------------------------------------------

// Usage: <input type="text" onkeyup="timlib_prevent_carrot(event,this);">
// -----------------------------------------------------------------------
function timlib_prevent_carrot( theEvent, element )
{
	var len;
	var without_character;
	var new_value;

	// Replace ^ with **
	// -----------------
	if ( theEvent.shiftKey && theEvent.keyCode == 54 )
	{
		len = element.value.length;
		without_character = element.value.substr( 0, len - 1 );
		new_value = without_character.concat( "**" );
		alert( "Sorry. Will replace with '**'" );
		element.value = new_value;
	}

	return true;
}

function timlib_count_lines( s )
{
	var number_lines = 1;
	var code;
	var i;
	var local_string;

	local_string = new String( s );

	for( i = 0; i < local_string.length; i++ )
	{
		code = local_string.charCodeAt( i );
		if ( code == 10 ) number_lines++;
	}
	return number_lines;
}

function timlib_check_notepad_size( element, max_length )
{
	var element_length;

	element_length = element.value.length;

	if ( element_length > max_length )
	{
		var truncating_length;

		truncating_length = 0 - ( max_length - element_length );

		alert(
		'Warning: max length is ' +
		max_length +
		'; will truncate last ' +
		truncating_length +
		' characters.' );
	}
	return true;
}

function timlib_get_drop_down_element_populated( options )
{
	return timlib_drop_down_element_populated( options );
}

function timlib_drop_down_element_populated( options )
{
	var value;
	var i;

	if ( options == undefined ) return false;

	value =
		// Returns 'undefined' if false.
		// -----------------------------
		timlib_drop_down_element_value(
			options );

	if ( value != 'undefined' && value != 'select' )
		return true;
	else
		return false;
}

function timlib_get_multi_select_drop_down_element_populated( options )
{
	return timlib_multi_select_drop_down_element_populated( options );
}

function timlib_multi_select_drop_down_element_populated( options )
{
	var value;
	var i;

	if ( options == undefined ) return false;

	for( i = 0; i < options.length; i++ )
	{
		value = options[ i ].value;

		if ( value != undefined
		&&   value != 'undefined'
		&&   value != 'select'
		&&   value != "" )
		{
			return true;
		}
	}
	return false;
}

function timlib_exists( element_name, element_name_list )
{
	var element_name_compare;

	for ( var i = 0; i < element_name_list.length; i++ )
	{
		element_name_compare = element_name_list[ i ];
		if ( element_name_compare == element_name ) return true;
	}
	return false;
}

function timlib_get_element( search_element_name )
{
	return timlib_element( search_element_name );
}

function timlib_row_element( attribute_name, row_number )
{
	var element;
	var element_name;
	var form_number;

	if ( row_number <= 0 )
	{
		element_name = attribute_name;
	}
	else
	{
		element_name = attribute_name + "_" + row_number;
	}

	for (   form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		element =
			timlib_form_element(
				element_name,
				form_number );

		if ( element != false ) return element;
	}

	element_name = 'from_' + attribute_name;

	for (   form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		element =
			timlib_form_element(
				element_name,
				form_number );

		if ( element != false ) return element;
	}

	return false;
}

// To be replaced with document.getElementById()
// ---------------------------------------------
function timlib_element( search_element_name )
{
	var element;
	var i;
	var form;
	var modified_element_name;

	for (   var form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		element =
			timlib_form_element(
				search_element_name,
				form_number );

		if ( element != false ) return element;
	}

	modified_element_name = 'from_' + search_element_name;

	for (   var form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		element =
			timlib_form_element(
				modified_element_name,
				form_number );

		if ( element != false ) return element;
	}

	modified_element_name = 'from_' + search_element_name + '_0';

	for (   var form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		element =
			timlib_form_element(
				modified_element_name,
				form_number );

		if ( element != false ) return element;
	}

	modified_element_name = search_element_name + '_0';

	for (   var form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		element =
			timlib_form_element(
				modified_element_name,
				form_number );

		if ( element != false ) return element;
	}

	return false;
}

function timlib_get_form_element( search_element_name, form_number )
{
	return timlib_form_element( search_element_name, form_number );
}

function timlib_form_element( search_element_name, form_number )
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

	return false;
}

function timlib_get_radio_selected_value( search_element_name )
{
	return timlib_radio_selected_value( search_element_name );
}

function timlib_radio_selected_value( search_element_name )
{
        var element_name;
	var return_element_value;
	var element;
	var i;
	var form;

        for (   var form_number = 0;
                form_number < document.forms.length;
                form_number++ )
        {
		form = document.forms[ form_number ];

		for( i = 0; i < form.elements.length; i++ )
                {
			element = form.elements[ i ];
                        element_name = element.name;

                        if ( element.type == 'radio'
			&&   element_name == search_element_name
			&&   element.checked == true )
                        {
				return_element_value = element.value;
				return return_element_value;
                        }
                }
        }
        return "";
}

function timlib_get_drop_down_element_value( options )
{
	return timlib_drop_down_element_value( options );
}

// Returns 'undefined' if false.
// -----------------------------
function timlib_drop_down_element_value( options )
{
        var return_value;

	if ( options == undefined ) return 'undefined';

        for( var i = 0; i < options.length; i++ )
        {
                if ( options[ i ].selected )
                {
                        return_value = options[ i ].value;
                        return return_value;
                }
        }
	return 'undefined';
}

function timlib_set_drop_down_element_value( options, value )
{
	if ( options == undefined ) return true;

        for( var i = 0; i < options.length; i++ )
        {
                if ( options[ i ].selected )
			options[ i ].selected = false;
        }
        for( var i = 0; i < options.length; i++ )
        {
		if ( options[ i ].value == value )
			options[ i ].selected = true;
        }
	return true;
}

function timlib_gray_mutually_exclusive_drop_downs( element_name_list_string )
{
	var element_name_list;
	var select_element_name;
	var original_element_name;
	var gray_element_name;
	var element;
	var value;
	var list_length;

	element_name_list =
		element_name_list_string.split( ',' );

	list_length = element_name_list.length;

	for ( var i = 0; i < list_length; i++ )
	{
		select_element_name = element_name_list[ i ];

		element = timlib_element( select_element_name );

		element.disabled = 0;

		if ( element.type == 'select-multiple' )
		{
			original_element_name =
				"original_" + select_element_name;

			element = timlib_element( original_element_name );

			element.disabled = 0;
		}
	}

	for ( var i = 0; i < list_length; i++ )
	{
		select_element_name = element_name_list[ i ];

		element = timlib_element( select_element_name );

		if ( element.type == 'select-multiple' )
		{
			value =
			timlib_multi_select_drop_down_element_populated(
					element.options );
		}
		else
		{
			value =
			timlib_drop_down_element_value(
					element.options );
		}

		if ( value != 'select'
		&&   value != ''
		&&   value != 'undefined'
		&&   value != false
		&&   value != undefined )
		{
			// Got one, so disable the others and leave.
			// -----------------------------------------
			for( var j = 0; j < list_length; j++ )
			{
				gray_element_name = element_name_list[ j ];

				if ( gray_element_name == select_element_name )
					continue;

				element =
					timlib_element(
						gray_element_name );

				element.disabled = 1;

				if ( element.type == 'select-multiple' )
				{
					gray_element_name =
						"original_" + gray_element_name;
					element =
						timlib_element(
							gray_element_name );
					element.disabled = 1;
				}
			}
			break;
		}
	}

	return true;
}

function timlib_get_delimiter( string )
{
	return timlib_delimiter( string );
}

function timlib_delimiter( string )
{
	var length;
	var n;

	length = string.length;

	for( n = 0; n < string.length; n++ )
	{
		if ( string.charAt( n ) == '/' )
			return '/';
		else
		if ( string.charAt( n ) == ':' )
			return ':';
		else
		if ( string.charAt( n ) == '^' )
			return '^';
		else
		if ( string.charAt( n ) == '|' )
			return '|';
		else
		if ( string.charAt( n ) == '.' )
			return '.';
		else
		if ( string.charAt( n ) == '-' )
			return '-';
	}
	return '';
}

function timlib_get_non_minus_delimiter( string )
{
	return timlib_non_minus_delimiter( string );
}

function timlib_non_minus_delimiter( string )
{
	var length;
	var n;

	length = string.length;

	for( n = 0; n < string.length; n++ )
	{
		if ( string.charAt( n ) == '/' )
			return '/';
		else
		if ( string.charAt( n ) == ':' )
			return ':';
		else
		if ( string.charAt( n ) == '^' )
			return '^';
		else
		if ( string.charAt( n ) == '|' )
			return '|';
		else
		if ( string.charAt( n ) == '.' )
			return '.';
	}
	return '';
}

function timlib_set_all_push_buttons(
		master_element,
		prefix )
{
	var element;
	var i;
	var form;
	var form_number;
	var prefix_length;
	var master_element_checked;

	prefix_length = prefix.length;
	master_element_checked = master_element.checked;

	for (   form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		form = document.forms[ form_number ];

		for( i = 0; i < form.elements.length; i++ )
		{
			element = form.elements[ i ];

			if ( element.name == master_element.name ) continue;

			if (	element.name.substr( 0, prefix_length ) ==
				prefix )
                        {
				element.checked = master_element_checked;
			}
		}
	}
	return true;
}

function timlib_get_index( attribute_name, element_name )
{
	return timlib_index( attribute_name, element_name );
}

function timlib_index( attribute_name, element_name )
{
	var index_string;
	var len;

	len = attribute_name.length;

	if ( element_name.charAt( len ) != '_' )
	{
		return -1;
	}

	index_string = element_name.substr( len + 1 );
	return index_string;
}

function timlib_remove_character( input_string, remove_character )
{
	var len;
	var i;
	var ch;
	var return_string;

	len = input_string.length;
	return_string = "";

	for ( i = 0; i < len; i++ )
	{
		ch = input_string.charAt( i );

		if ( ch != remove_character )
		{
			return_string = return_string + ch;
		}
	}

	return return_string;
}

function timlib_remove_index( element_name )
{
	var len;
	var i;
	var ch;
	var is_number;
	var is_underbar;

	len = element_name.length;

	for ( i = len - 1; i; i-- )
	{
		ch = element_name.charAt( i );

		if ( ch >= 0 && ch <= 9 )
			is_number = 1;
		else
			is_number = 0;

		if ( ch == '_' )
			is_underbar = 1;
		else
			is_underbar = 0;

		if ( !is_number && !is_underbar ) return element_name;

		if ( is_underbar ) break;
	}

	if ( i == 0 ) return element_name;

	return element_name.substr( 0, i );

}

function timlib_element_value_set(
		element_name,
		element_value  )
{
	var element;

	element = timlib_element( element_name );

	if ( element == false )
	{
		alert( 'Cannot find element_name = ' + element_name );
		return false;
	}

	element.value = element_value;

	return true;
}

function timlib_history_forward()
{
	history.forward();

	return true;
}

var delete_warned_already = false;

function timlib_delete_button_warning()
{
	if ( !delete_warned_already )
	{
		alert( "Warning: you pressed the delete button." );
	}

	delete_warned_already = true;

	return true;
}

function timlib_push_button_set_all(
		operation_name,
		form_number )
{
	var set_all_element;
	var push_button_element_name;
	var push_button_element;
	var row;

	set_all_element =
		timlib_form_element(
			operation_name, form_number );

	for( row = 1; ; row++ )
	{
		push_button_element_name = operation_name + "_" + row;

		push_button_element =
			timlib_form_element(
				push_button_element_name,
				form_number );

		if ( !push_button_element ) break;

		push_button_element.checked = set_all_element.checked;
	}

	return true;
}

function timlib_password_view(
		checkbox_element,
		password_element_name,
		password_display_element_name )
{
	var password_element;
	var password_display_element;

	password_element =
		timlib_element(
			password_element_name );

	password_display_element =
		timlib_element(
			password_display_element_name );

	if ( checkbox_element.checked == true )
	{
		password_display_element.value =
			password_element.value;
	}
	else
	{
		if ( password_display_element.value != '' )
		{
			// Synchronize with ROW_SECURITY_ASTERISK
			// --------------------------------------
			password_display_element.value = "**********";
		}
	}

	return true;
}

function timlib_password_set(
		password_element_name,
		password_display_element_name )
{
	var password_element;
	var password_display_element;

	password_element =
		timlib_element(
			password_element_name );

	password_display_element =
		timlib_element(
			password_display_element_name );

	password_element.value = password_display_element.value;

	return true;
}

function timlib_password_compare( password, password_compare )
{
	var password_element;
	var password_compare_element;

	password_element = timlib_element( password );

	password_compare_element =
		timlib_element(
			password_compare );

	if ( password_element.value != '/'
	&&   password_element.value != password_compare_element.value )
	{
		alert( 'Warning: the passwords do not match.' );
		return false;
	}

	return true;
}

function timlib_wait_start()
{
	document.body.style.cursor = 'wait';
	return true;
}

function timlib_wait_over()
{
	const frames = window.parent.frames;

	for ( let i = 0; i < frames.length; i++ )
	{
		frames[ i ].document.body.style.cursor = "default";
	}
	return true;
}

