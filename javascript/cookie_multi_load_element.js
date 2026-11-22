// $APPASERVER_HOME/javascript/cookie_multi_load_element.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class cookie_multi_load_element_array {
constructor(
	recall_equal_delimiter,
	recall_block_delimiter,
	recall_multi_delimiter,
	cookie_load_raw_key_string )
{
	this.array = new Array();

	let cookie_block_string_array =
		cookie_load_raw_key_string.split(
			recall_block_delimiter );

	for ( const cookie_block_string of cookie_block_string_array )
	{
		let multi_load_element =
			new cookie_multi_load_element(
				recall_equal_delimiter,
				recall_multi_delimiter,
				cookie_block_string );

		this.array.push( multi_load_element );
	}
}
}

class cookie_multi_load_element {
constructor(
	recall_equal_delimiter,
	recall_multi_delimiter,
	cookie_block_string )
{
	this.cookie_multi_load_element_split =
		new cookie_multi_load_element_split(
			recall_equal_delimiter,
			cookie_block_string );

	this.original_name =
		cookie_multi_load_element_original_name(
			this.cookie_multi_load_element_split.name
				/* element_name */ );

	this.datum_array =
		cookie_multi_load_element_datum_array(
			recall_multi_delimiter,
			this.cookie_multi_load_element_split.
				datum_array_string );
}
}

function cookie_multi_load_element_original_name(
	element_name )
{
	return "original_" + element_name;
}

function cookie_multi_load_element_datum_array(
	recall_multi_delimiter,
	datum_array_string )
{
	return datum_array_string.split( recall_multi_delimiter );
}

class cookie_multi_load_element_split {
constructor(
	recall_equal_delimiter,
	cookie_block_string )
{
	let array = cookie_block_string.split( recall_equal_delimiter );
	this.name = array[ 0 ];
	this.datum_array_string = array[ 1 ];
}
}
