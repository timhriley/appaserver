//
// post_generic_load.js
// --------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

function after_field(	position_element_name,
			constant_element_name,
			default_position )
{
	var position_element;
	var constant_element;
	var constant_value;
	var position_value;

	position_element = timlib_get_element( position_element_name );
	position_element.disabled = 0;
	position_value = position_element.value;

	constant_element = timlib_get_element( constant_element_name );
	constant_value = constant_element.value;

	// If constant_value is populated
	// ------------------------------
	if (   constant_value != undefined
	&&     constant_value != 'undefined'
	&&     constant_value != 'select'
	&&     constant_value != '' )
	{
		position_element.disabled = 1;
	}
	else
	// ------------------------------------------------------
	// If position_value is empty and constant_value is empty
	// ------------------------------------------------------
	if (   position_value == undefined
	||     position_value == 'undefined'
	||     position_value == ''
	||     parseInt( position_value ) < 1 )
	{
		if ( default_position )
		{
			position_element.value = default_position;
		}
	}

	return true;

} // after_field()

function after_ignore(	ignore_element,
			position_element_name,
			constant_element_name )
{
	var position_element;
	var constant_element;

	position_element = timlib_get_element( position_element_name );
	constant_element = timlib_get_element( constant_element_name );

	if ( ignore_element.checked )
	{
		position_element.disabled = 1;
		constant_element.disabled = 1;
	}
	else
	{
		if ( constant_element.value == undefined
		||   constant_element.value == 'undefined'
		||   constant_element.value == 'select'
		||   constant_element.value == '' )
		{
			position_element.disabled = 0;
			constant_element.disabled = 0;
		}
		else
		{
			position_element.disabled = 1;
			constant_element.disabled = 0;
		}
	}

	return true;
} // after_ignore()

