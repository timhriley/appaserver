// $APPASERVER_HOME/src_predictive/post_change_bank_upload.js
// ----------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ----------------------------------------------------------------

function post_change_bank_upload_sort_one()
{
	var row;
	var bank_running_balance_element;
	var bank_running_balance_element_name;

	row = 1;

	while( true )
	{
		bank_running_balance_element_name =
			'bank_running_balance_' + row;

		bank_running_balance_element =
			timlib_get_element(
				bank_running_balance_element_name );

		if ( bank_running_balance_element == '' ) break;

		bank_running_balance_element.value = 0.0;

		row++;
	}
}

function post_change_bank_upload_sort_accumulate()
{
	var row;
	var bank_running_balance_element;
	var bank_running_balance_element_name;
	var bank_amount_element;
	var bank_amount_element_name;
	var bank_running_balance_value;
	var bank_amount_value;

	bank_running_balance_element_name = 'bank_running_balance_1';

	bank_running_balance_element =
		timlib_get_form_element(
			bank_running_balance_element_name, 0 );

	bank_running_balance_value =
		trim_character(
			bank_running_balance_element.value,
			',' ) - 0.0;

	row = 2;

	while( true )
	{
		bank_amount_element_name =
			'bank_amount_' + row;

		bank_amount_element =
			timlib_get_element(
				bank_amount_element_name );

		if ( bank_amount_element == '' ) break;

		bank_amount_value =
			trim_character(
				bank_amount_element.value,
				',' ) - 0.0;

		bank_running_balance_element_name =
			'bank_running_balance_' + row;

		bank_running_balance_element =
			timlib_get_element(
				bank_running_balance_element_name );

		bank_running_balance_value += bank_amount_value;

		bank_running_balance_element.value =
			bank_running_balance_value.toFixed(2);

		row++;
	}
}

function post_change_bank_upload( state, row )
{
	var radio_one_selected;

	if ( state != 'sort' ) return true;

	if ( row == 0 )
	{
		post_change_bank_upload_sort_accumulate()
		return true;
	}

	if ( row == 1 )
	{
		post_change_bank_upload_sort_one();
		return true;
	}

	radio_one_selected =
		timlib_get_radio_selected_value(
			"move" );

	if ( radio_one_selected == "move_1" )
	{
		post_change_bank_upload_sort_one();
		return true;
	}

	post_change_bank_upload_sort_accumulate();

	return true;
}

