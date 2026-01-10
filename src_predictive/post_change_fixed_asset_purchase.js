// $APPASERVER_HOME/src_predictive/post_change_fixed_asset_purchase.js
// -------------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// -------------------------------------------------------------------
function post_change_fixed_asset_purchase( row )
{
	var element_name;
	var element;

	if ( row <= 0 )
	{
		post_change_fixed_asset_purchase_each_row();
		// return true; <-- Need to pass through for insert screen.
	}

	// Turn on each edit.
	// ------------------
	element_name = 'estimated_useful_life_years_' + row;
	element = timlib_get_element( element_name );

	// All done
	// --------
	if ( element == "" ) return false;

	element.disabled = false;

	element_name = 'units_produced_so_far_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	element_name = 'estimated_useful_life_units_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	element_name = 'estimated_residual_value_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	element_name = 'declining_balance_n_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	post_change_fixed_asset_purchase_depreciation( row )
  
	return true;
}

function post_change_fixed_asset_purchase_depreciation( row )
{
	var element_name;
	var element;
	var depreciation_method_element_value;

	element_name = 'depreciation_method_' + row;

	element = timlib_get_element( element_name );

	depreciation_method_element_value =
		// ---------------------------------
		// Returns 'undefined' if not found.
		// ---------------------------------
		timlib_get_drop_down_element_value(
			element.options );

	if ( depreciation_method_element_value == 'undefined' )
	{
		return true;
	}

	if ( depreciation_method_element_value == 'straight_line' )
	{
		element_name = 'units_produced_so_far_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'estimated_useful_life_units_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'double_declining_balance' )
	{
		element_name = 'units_produced_so_far_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'estimated_useful_life_units_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'n_declining_balance' )
	{
		element_name = 'units_produced_so_far_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'estimated_useful_life_units_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'sum_of_years_digits' )
	{
		element_name = 'units_produced_so_far_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'estimated_useful_life_units_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}
	else
	if ( depreciation_method_element_value == 'units_of_production' )
	{
		element_name = 'estimated_useful_life_years_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;

		element_name = 'declining_balance_n_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}

	return true;
}

function post_change_fixed_asset_purchase_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = post_change_fixed_asset_purchase( row );
		if ( results == false ) break;
		row++;
	}

	return true;
}
