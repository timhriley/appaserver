// $APPASERVER_HOME/src_predictive/post_change_employee.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------
function post_change_employee( row )
{
	var element_name;
	var element;
	var disable_hourly_wage = true;
	var disable_period_salary = true;
	var hourly_wage_is_slash = false;
	var period_salary_is_slash = false;
	var nothing_is_populated = true;

	// If from document onload
	// -----------------------
	if ( row == -1 )
	{
		post_change_employee_each_row();
		return true;
	}

	// Turn on edits
	// -------------
	element_name = 'hourly_wage_' + row;
	element = timlib_get_element( element_name );

	if ( element == "" ) return false;

	element.disabled = false;

	element_name = 'period_salary_' + row;
	element = timlib_get_element( element_name );
	element.disabled = false;

	// Check hourly_wage
	// -----------------
	element_name = 'hourly_wage_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" )
	{
		hourly_wage_is_slash = true;
	}
	else
	if ( element.value != "" )
	{
		disable_hourly_wage = false;
		nothing_is_populated = false;
	}

	// Check period_salary
	// -------------------
	element_name = 'period_salary_' + row;
	element = timlib_get_element( element_name );

	if ( element.value == "/" )
	{
		period_salary_is_slash = true;
	}
	else
	if ( element.value != "" )
	{
		disable_period_salary = false;
		nothing_is_populated = false;
	}

	/* Disable widgets */
	/* --------------- */
	if ( disable_hourly_wage
	&&   !hourly_wage_is_slash
	&&   !nothing_is_populated )
	{
		element_name = 'hourly_wage_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}

	if ( disable_period_salary
	&&   !period_salary_is_slash
	&&   !nothing_is_populated )
	{
		element_name = 'period_salary_' + row;
		element = timlib_get_element( element_name );
		element.disabled = true;
	}

	return true;
}

function post_change_employee_each_row()
{
	var row;
	var results;

	row = 1;
	while( true )
	{
		results = post_change_employee( row );
		if ( results == false ) break;
		row++;
	}

	return true;
}
