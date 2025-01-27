// $APPASERVER_HOME/javascript/validate_date.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function validate_date( element )
{
	var result;
	var element_value;
	var date_value;
	var time_value;
	var component_array;

	element_value = element.value;

	component_array = element_value.split( ' ' );

	if ( component_array.length == 2 )
	{
		date_value = component_array[ 0 ];
		time_value = component_array[ 1 ];
	}
	else
	{
		date_value = element_value;
		time_value = 'NULL';
	}

	if ( time_value != 'NULL' )
	{
		result = time_value.search( ':' );

		if ( result != -1 )
		{
			validate_time_with_colons( time_value );
		}
	}

	result = date_value.search( '/' );

	if ( result != -1 )
	{
		return validate_date_with_slashes( date_value );
	}

	result = date_value.search( '-' );

	if ( result != -1 )
	{
		if ( date_value == "0000-00-00" ) return true;
		return validate_date_with_dashes( date_value );
	}

	return true;
}

function validate_time( element )
{
	var time_prefix;

	if ( element.value == "null" ) return true;

	if ( element.value.length >= 4 )
		time_prefix = "";
	else
	if ( element.value.length == 3 )
		time_prefix = "0";
	else
	if ( element.value.length == 2 )
		time_prefix = "00";
	else
	if ( element.value.length == 1 )
		time_prefix = "000";
	else
	if ( element.value.length == 0 )
		time_prefix = "";

	element.value = time_prefix + element.value;

	if ( element.value.length != 0
	&&   ( element.value > "2359" 
	||     element.value < "0000"
	||     ( element.value.match( /[A-z]/ ) != null ) ) )
	{
		alert( 'Invalid format for time.' );
		element.value = element.defaultValue;
	}
	return true;
}

function validate_time_with_colons( time_value )
{
	var component_array;
	var hour;
	var minute;
	var second;
	var display_message = false;

	component_array = time_value.split( ':' );

	if ( component_array.length != 3 )
	{
		display_message = true;
	}
	else
	{
		hour = component_array[ 0 ];
		minute = component_array[ 1 ];
		second = component_array[ 2 ];

		if ( !validate_time_hour( hour ) ) display_message = true;
		if ( !validate_time_minute( minute ) ) display_message = true;
		if ( !validate_time_second( second ) ) display_message = true;
	}

	if ( display_message == true )
	{
		alert( 'Warning: Invalid format for time.' );
	}

	return true;
}

function validate_date_with_dashes( value )
{
	var component_array;
	var year;
	var month;
	var day;
	var display_message = false;

	component_array = value.split( '-' );

	if ( component_array.length != 3 )
	{
		display_message = true;
	}
	else
	{
		year = component_array[ 0 ];
		month = component_array[ 1 ];
		day = component_array[ 2 ];

		if ( month.match( /[A-z][A-z]*/ ) != null )
		{
			return validate_date_military( value );
		}

		if ( !validate_date_year( year ) )
		{
			display_message = true;
		}

		if ( !validate_date_month( month ) )
		{
			display_message = true;
		}

		if ( !validate_date_day( day, month, year ) )
		{
			display_message = true;
		}
	}

	if ( display_message == true )
	{
		alert( 'Warning: Invalid format for date.' );
	}

	return true;
}

function validate_date_with_slashes( value )
{
	var component_array;
	var year;
	var month;
	var day;
	var display_message = false;

	component_array = value.split( '/' );

	if ( component_array.length != 3 )
	{
		display_message = true;
	}
	else
	{
		month = component_array[ 0 ];
		day = component_array[ 1 ];
		year = component_array[ 2 ];

		if ( !validate_date_year( year ) )
		{
			display_message = true;
		}

		if ( !validate_date_month( month ) )
		{
			display_message = true;
		}

		if ( !validate_date_day( day, month, year ) )
		{
			display_message = true;
		}
	}

	if ( display_message == true )
	{
		alert( 'Warning: Invalid format for date.' );
	}

	return true;
}

function validate_date_military( value )
{
	var component_array;
	var year;
	var month;
	var day;
	var display_message = false;

	component_array = value.split( '-' );

	if ( component_array.length != 3 )
	{
		display_message = true;
	}
	else
	{
		day = component_array[ 0 ];
		month = component_array[ 1 ];
		year = component_array[ 2 ];

		if ( !validate_date_year( year ) )
		{
			display_message = true;
		}

		if ( !validate_military_month( month ) )
		{
			display_message = true;
		}

		if ( !validate_date_day( day, month, year ) )
		{
			display_message = true;
		}
	}

	if ( display_message == true )
	{
		alert( 'Warning: Invalid format for date.' );
	}

	return true;
}

function validate_date_year( year )
{
	if ( year.match( /[A-z][A-z]*/ ) != null )
	{
		alert( 'The year must be all digits.' );
		return false;
	}

	if ( year.length == 2 )
	{
		return true;
	}
	else
	if ( year.length == 4 )
	{
		if ( year < 1912 )
		{
			alert( 'The year seems excessively early.' );
			return false;
		}
	}
	else
	{
		alert( 'Invalid year.' );
		return false;
	}

	return true;
}

function validate_military_month( month )
{
	var month_lower;

	month_lower = month.toLowerCase();

	if ( month_lower == "jan" ) return true;
	if ( month_lower == "feb" ) return true;
	if ( month_lower == "mar" ) return true;
	if ( month_lower == "apr" ) return true;
	if ( month_lower == "may" ) return true;
	if ( month_lower == "jun" ) return true;
	if ( month_lower == "jul" ) return true;
	if ( month_lower == "aug" ) return true;
	if ( month_lower == "sep" ) return true;
	if ( month_lower == "oct" ) return true;
	if ( month_lower == "nov" ) return true;
	if ( month_lower == "dec" ) return true;

	alert( 'Invalid military month.' );
	return false;
}

function validate_date_month( month )
{
	if ( month.match( /[A-z][A-z]*/ ) != null )
	{
		alert( 'The month must be all digits.' );
		return false;
	}

	if ( month < 1 || month > 12 )
	{
		alert( 'Invalid month.' );
		return false;
	}

	return true;
}

function validate_date_day( day, month, year )
{
	var max_day;

	if ( day.match( /[A-z][A-z]*/ ) != null )
	{
		alert( 'The day must be all digits.' );
		return false;
	}

	if ( month != 2 )
	{
		if ( month == 1
		||   month == 3
		||   month == 5
		||   month == 7
		||   month == 8
		||   month == 10
		||   month == 12 )
		{
			max_day = 31;
		}
		else
		{
			max_day = 30;
		}
	}
	else
	{
		if ( ( year % 4 ) == 0 )
		{
			max_day = 29;
		}
		else
		{
			max_day = 28;
		}
	}

	if ( day < 1 || day > max_day )
	{
		alert( 'Invalid day.' );
		return false;
	}

	return true;
}

function validate_time_hour( hour )
{
	if ( hour.match( /[A-z][A-z]*/ ) != null )
	{
		alert( 'The hour must be all digits.' );
		return false;
	}

	if ( hour < 0 || hour > 23 )
	{
		alert( 'The hour must be between 0 and 23.' );
		return false;
	}

	return true;
}

function validate_time_minute( minute )
{
	if ( minute.match( /[A-z][A-z]*/ ) != null )
	{
		alert( 'The minute must be all digits.' );
		return false;
	}

	if ( minute < 0 || minute > 59 )
	{
		alert( 'The minute must be between 0 and 59.' );
		return false;
	}

	return true;
}

function validate_time_second( second )
{
	if ( second.match( /[A-z][A-z]*/ ) != null )
	{
		alert( 'The second must be all digits.' );
		return false;
	}

	if ( second < 0 || second > 59 )
	{
		alert( 'The second must be between 0 and 59.' );
		return false;
	}

	return true;
}

