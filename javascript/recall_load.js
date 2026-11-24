// $APPASERVER_HOME/javascript/recall_load.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class recall_load {
constructor(
	form,
	cookie_key,
	recall_equal_delimiter /* Probably == */,
	recall_block_delimiter /* Probably ~~ */ )
{
	let cookie_string = document.cookie;

	if ( !cookie_string.length ) return false;

	this.cookie_load_raw_array =
		cookie_load_raw_array(
			cookie_string,
			';' /* cookie_load_raw_delimiter */ );

	this.cookie_load_raw_key_string =
		cookie_load_raw_key_string(
			cookie_key,
			this.cookie_load_raw_array );

	if ( !this.cookie_load_raw_key_string ) return false;

	this.cookie_load_element_array =
		new cookie_load_element_array(
			recall_equal_delimiter,
			recall_block_delimiter,
			this.cookie_load_raw_key_string );

	for (	const cookie_load_element of
		this.cookie_load_element_array.array )
	{
		let element =
			document.getElementById(
				cookie_load_element.name );

		/* If the element was removed */
		/* -------------------------- */
		if ( !element ) continue;

		if ( element.type == 'select-one' )
		{
			recall_load_set_drop_down_element_selected(
				element,
				cookie_load_element.datum_unescape );
		}
		else
		if ( element.type == 'checkbox' )
		{
			recall_load_set_checkbox_element(
				element,
				cookie_load_element.datum_unescape );
		}
		else
		{
	     		element.value = cookie_load_element.datum_unescape;
		}
	}
}
}

function recall_load_set_drop_down_element_selected(
	element,
	datum_unescape )
{
	for ( const option of element.options )
	{
		if ( option.value == datum_unescape )
		{
			option.selected = true;
			return true;
		}
	}

	return false;
}

function recall_load_set_checkbox_element(
	element,
	datum_unescape )
{
	if ( datum_unescape == 'Y' ) element.checked = true;

	return true;
}
