// $APPASERVER_HOME/javascript/recall_multi_load.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class recall_multi_load {
constructor(
	form,
	cookie_key,
	recall_equal_delimiter,
	recall_block_delimiter,
	recall_multi_delimiter,
	recall_multi_index_delimiter )
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

	this.cookie_multi_load_element_array =
		new cookie_multi_load_element_array(
			recall_equal_delimiter,
			recall_block_delimiter,
			recall_multi_delimiter,
			this.cookie_load_raw_key_string );

	for (	const cookie_multi_load_element of
		this.cookie_multi_load_element_array.array )
	{
		let element =
			document.getElementById(
				cookie_multi_load_element.
					cookie_multi_load_element_split.
					name );

		/* If the element was removed */
		/* -------------------------- */
		if ( !element ) continue;

		let original_element =
			document.getElementById(
				cookie_multi_load_element.
					original_name );

		if ( !original_element ) continue;

		recall_multi_load_set(
			cookie_multi_load_element.datum_array,
			element,
			original_element );

		button_multi_move_right(
			original_element.name,
			element.name,
			recall_multi_index_delimiter );
	}
}
}

function recall_multi_load_set(
	datum_array,
	element,
	original_element )
{
	for ( const datum of datum_array )
	{
		recall_multi_load_set_selected(
			original_element.options
				/* option_array */,
			datum );
	}
}

function recall_multi_load_set_selected(
	option_array,
	datum )
{
	for ( const option of option_array )
	{
		if ( option.value == datum )
		{
			option.selected = true;
			return true;
		}
	}

	return false;
}
