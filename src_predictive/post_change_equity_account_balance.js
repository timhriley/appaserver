// $APPASERVER_HOMEsrc_predictive/post_change_equity_account_balance.js
// --------------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// --------------------------------------------------------------------

function parse_classification( value )
{
	var classification;
	var string_array;
	var classification_part;

	classification = "";

	string_array = value.split( '[' );

	classification_part = string_array[ 1 ];
	string_array = classification_part.split( ']' );
	classification = string_array[ 0 ];

	return classification;
}

function post_change_equity_account_balance()
{
	var element;
	var value;
	var classification;

	element = timlib_get_element( "market_value" );
	element.disabled = false;

	element = timlib_get_element( "share_price" );
	element.disabled = false;

	element = timlib_get_element( "share_quantity_change" );
	element.disabled = false;

	element = timlib_get_element( "share_quantity_balance" );
	element.disabled = false;

	element =
		timlib_get_element(
			"full_name^street_address^account_number" );

	classification = parse_classification( element.value );

	if ( classification == "" )
	{
		return true;
	}

	if ( classification == "checking"
	||   classification == "savings"
	||   classification == "certificate"
	||   classification == "money_market" )
	{
		element = timlib_get_element( "market_value_0" );
		element.disabled = true;

		element = timlib_get_element( "share_price_0" );
		element.value = "1";

		// element = timlib_get_element( "share_price" );
		// element.disabled = true;
	}

	element = timlib_get_element( "share_quantity_change" );
	if ( element.value > 0 )
	{
		element = timlib_get_element( "share_quantity_balance" );
		element.disabled = true;
	}

	element = timlib_get_element( "share_quantity_balance" );
	if ( element.value > 0 )
	{
		element = timlib_get_element( "share_quantity_change" );
		element.disabled = true;
	}

	element = timlib_get_element( "share_price" );
	if ( element.value > 0 )
	{
		element = timlib_get_element( "market_value" );
		element.disabled = true;
	}

	return true;
}

