//
// post_change_order_entry.js
// ---------------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function populate_inventory_sales_extension(	inventory_element_name,
						quantity_element_name,
						extension_element_name )
{
	var inventory_element;
	var quantity_element;
	var extension_element;
	var inventory_element_value;
	var price_value;

	inventory_element = timlib_get_element( inventory_element_name );
	quantity_element = timlib_get_element( quantity_element_name );
	extension_element = timlib_get_element( extension_element_name );

	inventory_element_value =
		timlib_get_drop_down_element_value(
			inventory_element.options );
	price_value = post_change_get_price_value( inventory_element_value );
	extension_element.value = quantity_element.value * price_value;
}

function set_shipping_address( row )
{
	var shipping_element_value;
	var billing_element_value;
	var shipping_element_name;
	var billing_element_name;

	shipping_element_name = 'shipping_address_same_as_billing_yn_' + row;
	shipping_element_value = post_change_get_value( shipping_element_name );

	if ( shipping_element_value == 'y' )
	{
		shipping_element_name = 'shipping_street_address_' + row;
		billing_element_name = 'billing_street_address_' + row;
		billing_element_value =
			post_change_get_value( billing_element_name );
		post_change_set_text_value(	shipping_element_name,
						billing_element_value );

		shipping_element_name = 'shipping_city_' + row;
		billing_element_name = 'billing_city_' + row;
		billing_element_value =
			post_change_get_value( billing_element_name );
		post_change_set_text_value(	shipping_element_name,
						billing_element_value );

		shipping_element_name = 'shipping_state_' + row;
		billing_element_name = 'billing_state_' + row;
		billing_element_value =
			post_change_get_value( billing_element_name );
		post_change_set_drop_down_element_value(
			shipping_element_name, billing_element_value );

		shipping_element_name = 'shipping_zip_code_' + row;
		billing_element_name = 'billing_zip_code_' + row;
		billing_element_value =
			post_change_get_value( billing_element_name );
		post_change_set_text_value(	shipping_element_name,
						billing_element_value );
	}
	return true;
} // set_shipping_address()

function post_change_get_value( search_element_name )
{
	var element_name;
	var return_value;
	var element;
	var i;

	for (	var form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		elements = document.forms[ form_number ].elements;

		for ( i = 0; i < elements.length; i++ )
		{
			element = elements[ i ];
			element_name = element.name;
			if ( element_name == search_element_name )
			{
				if ( element.type == 'select-one' )
				{
					return_value =
					post_change_get_drop_down_element_value(
						element.options );
					return return_value;
				}
				else
				{
					return_value = element.value;
					return return_value;
				}
			}
		}
	}
	return "";
} // post_change_get_value()

function post_change_set_text_value(	search_element_name,
					element_value )
{
	var element_name;
	var element;
	var form_number;
	var form;
	var i;

	if ( ( element_value == undefined )
	||   ( element_value == 'undefined' ) )
	{
		return;
	}

	for (	form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		form = document.forms[ form_number ];

		for( i = 0; i < form.elements.length; i++ )
		{
			element = form.elements[ i ];
			element_name = element.name;
			if ( element_name == search_element_name )
			{
				element.value = element_value;
				return;
			}
		}
	}
} // post_change_set_text_value()

function post_change_get_drop_down_element_value( options )
{
	var return_value;
	for( var i = 0; i < options.length; i++ )
	{
		if ( options[ i ].selected )
		{
			return_value = options[ i ].value;
			return return_value;
		}
	}
} // post_change_get_drop_down_element_value()

function post_change_set_drop_down_element_value(
					search_element_name, element_value )
{
	var element_name;
	var options;
	var i;
	var element;
	var form_number;
	var form;

	for (	form_number = 0;
		form_number < document.forms.length;
		form_number++ )
	{
		form = document.forms[ form_number ];

		for ( i = 0; i < form.elements.length; i++ )
		{
			element = form.elements[ i ];
			element_name = element.name;

			if ( element_name != search_element_name ) continue;

			options = element.options;

			for( var i = 0; i < options.length; i++)
			{
				if ( options[ i ].value == element_value )
				{
					options[ i ].selected = true;
					return;
				}
			}
		}
	}
} // post_change_set_drop_down_element_value()

// Looks like: 3/8 CRANK CASE BREATHER [46.00]
// -------------------------------------------
function post_change_get_price_value( inventory_element_value )
{
	var full_bracked_number;
	var right_bracked_number;
	var unbracked_number;
	var pattern;

	pattern = /\[.*\]/
	full_bracked_number = inventory_element_value.match( pattern );
	right_bracked_number = full_bracked_number[0].split( "[" );
	unbracked_number = right_bracked_number[1].split( "]" );
	return unbracked_number[0];
} // post_change_get_price_value( inventory_element )

