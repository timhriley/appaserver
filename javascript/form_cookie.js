// $APPASERVER_HOME/javascript/form_cookie.js
// ---------------------------------------------------------------
// This class connects html forms with
// cookies to save the keystrokes.
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function form_cookie(
		form,
		cookie_key,
		samesite,
		secure,
		element_name_list,
		with_load,
		is_multi,
		move_left_right_delimiter,
		multi_select_remember_delimiter )
{
	this.cookie =
		new cookie(
			document,
			cookie_key,
			240 /* hours */,
			'/' /* path */,
			'' /* domain */,
			samesite,
			secure );

	this.is_multi = is_multi;

	this.cookie.load();

	if ( with_load )
	{
		if ( is_multi )
		{
			this.cookie2form_multi(
				form,
				element_name_list,
				move_left_right_delimiter,
				multi_select_remember_delimiter );
		}
		else
		{
			this.cookie2form( form, element_name_list );
		}
	}
	return true;
}

form_cookie.prototype.cookie2form = function( form, element_name_list )
{
	var value;
	var element_name;
	var i;
	var element;

	for ( i = 0; i < form.elements.length; i++ )
	{
		element = form.elements[ i ];
		element_name = element.name;

		if ( timlib_exists( element_name, element_name_list ) )
		{
			if ( element_name == 'execute_yn' )
			{
				continue;
			}

			if ( element == undefined ) continue;

			value = this.cookie[ element_name ];

			if ( ( value == undefined )
			||   ( value == 'undefined' ) )
			{
				continue;
			}

			if ( element.type == 'select-one' )
			{
			     if ( value == '' ) value = 'select';

			     form_cookie_set_drop_down_element_selected(
					element,
					value );
			}
			else
			if ( element.type == 'checkbox' )
			{
			     form_cookie_set_checkbox_element(
					element,
					value );
			}
			else
			{
			     element.value = value;
			}
		}
	}
	return true;
}

form_cookie.prototype.cookie2form_multi = function(
		form,
		element_name_list,
		move_left_right_delimiter,
		multi_select_remember_delimiter )
{
	var element_name;
	var i;
	var element;

	element_name_list =
		form_cookie_trim_original_underbar_from_element_name_list(
			element_name_list );

	for ( i = 0; i < form.elements.length; i++ )
	{
		element = form.elements[ i ];
		element_name = element.name;

		if ( timlib_exists( element_name, element_name_list ) )
		{
			value = this.cookie[ element_name ];

			if ( ( value != undefined )
			&&   ( value != 'undefined' )
			&&   ( value != 'select' )
			&&   ( value != '' ) )
			{
				if ( element.type == 'select-multiple' )
				{
// alert( 'cookie2form_multi(): for multi element_name = (' + element_name + '), value = (' + value + ')' );
				form_cookie_set_multi_drop_down_element_value(
					element,
					value,
					move_left_right_delimiter,
					multi_select_remember_delimiter );
				}
			}
		}
	}
	return true;
}

form_cookie.prototype.store = function(
		form,
		element_name_list,
		multi_select_remember_delimiter )
{
	if ( this.is_multi )
	{
		this.store_multi(	form,
					element_name_list,
					multi_select_remember_delimiter );
		return true;
	}
	else
	{
		this.store_non_multi( form, element_name_list );
		return true;
	}
}

form_cookie.prototype.store_multi = function(
		form,
		element_name_list,
		multi_select_remember_delimiter )
{
	var value;
	var element_name;
	var i;
	var element;
//	var escaped_value;

	element_name_list =
		form_cookie_trim_original_underbar_from_element_name_list(
			element_name_list );

	for ( i = 0; i < form.elements.length; i++ )
	{
		element = form.elements[ i ];
		element_name = element.name;

		if ( timlib_exists( element_name, element_name_list ) )
		{
			if ( element.type == 'select-multiple' )
			{
				value =
				form_cookie_multi_drop_down_element_value(
					element,
					multi_select_remember_delimiter );

//				escaped_value = escape( value );

// alert( 'store_multi(): for element = ' + element_name + ', value = (' + value + ')' );

//				this.cookie[ element_name ] = escaped_value;
				this.cookie[ element_name ] = value;
			}
		}
	}
	this.cookie.store();
	return true;
}

form_cookie.prototype.store_non_multi = function( form, element_name_list )
{
	var value;
	var element_name;
	var i;
	var element;

	for ( i = 0; i < form.elements.length; i++ )
	{
		element = form.elements[ i ];
		element_name = element.name;

		if ( timlib_exists( element_name, element_name_list ) )
		{
			if ( element.type == 'select-one' )
			{
				value =
					form_cookie_drop_down_element_value(
						element );
			}
			else
			if ( element.type == 'checkbox' )
			{
				value =
					form_cookie_checkbox_element_value(
						element );
			}
			else
			{
				value = element.value;
			}

			if ( value == 'select'
			||   value == 'Select'
			||   value == null
			||   value == undefined
			||   value == 'undefined' )
			{
				value = '';
			}

// alert( 'store_non_multi(): for element = ' + element_name + ', value = (' + value + ')' );
			this.cookie[ element_name ] = value;
		}
	}

	this.cookie.store();
	return true;
}

form_cookie.prototype.display = function()
{
	this.cookie.display();
	return true;
}

function form_cookie_multi_drop_down_element_value(
		element,
		multi_select_remember_delimiter )
{
	var return_string = "";
	var first_time = 1;

	for( var i = 0; i < element.options.length; i++ )
	{
		if ( first_time )
			first_time = 0;
		else
			return_string += multi_select_remember_delimiter;
		return_string += element.options[ i ].value;
	}
	return return_string;
}

function form_cookie_drop_down_element_value( element )
{
	for( var i = 0; i < element.options.length; i++ )
		if ( element.options[ i ].selected )
			return element.options[ i ].value;
	return null;
}

function form_cookie_checkbox_element_value( element )
{
	if ( element.checked == true )
		return 'y';
	else
		return null;
}

function form_cookie_set_checkbox_element( element, value )
{
	if ( value == 'y' ) element.checked = true;
}

function form_cookie_set_drop_down_element_selected( element, value )
{
	for( var i = 0; i < element.options.length; i++ )
		if ( element.options[ i ].value == value )
			element.options[ i ].selected = true;
	return true;
}

function form_cookie_trim_original_underbar_from_element_name_list(
		element_name_list )
{
	var element_name;

	for ( var i = 0; i < element_name_list.length; i++ )
	{
		element_name = element_name_list[ i ];
		if ( element_name.substring( 0, 9 ) == "original_" )
		{
			element_name_list[ i ] =
				element_name.substring( 9, 99 );
		}
	}
	return element_name_list;
}

function form_cookie_set_multi_drop_down_element_value(
		element,
		element_value_list_string,
		move_left_right_delimiter,
		multi_select_delimiter )
{
	var element_value_list;
	var element_value;
	var list_length;
	var original_element;
	var original_element_name;

	original_element_name = "original_" + element.name;
	original_element = timlib_get_element( original_element_name );

	element_value_list =
		element_value_list_string.split( multi_select_delimiter );

	list_length = element_value_list.length;

	for ( var i = 0; i < list_length; i++ )
	{
		element_value =
			trim_trailing_piece(
				element_value_list[ i ],
				move_left_right_delimiter );

		form_cookie_set_drop_down_element_selected(
			original_element,
			element_value )
	}

	post_change_multi_select_move_right(
		original_element_name,
		element.name,
		move_left_right_delimiter );

	return true;
}

