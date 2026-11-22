// $APPASERVER_HOME/javascript/cookie_load_element.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class cookie_load_element_array {
constructor(
	recall_equal_delimiter,
	recall_block_delimiter,
	cookie_load_raw_string )
{
	this.array = new Array();

	let cookie_block_string_array =
		cookie_load_raw_string.split(
			recall_block_delimiter );

	for ( const cookie_block_string of cookie_block_string_array )
	{
		let cookie_piece_array =
			cookie_block_string.split(
				recall_equal_delimiter );

		let load_element =
			new cookie_load_element(
				cookie_piece_array[ 0 ] /* name */,
				cookie_piece_array[ 1 ] /* datum */ );

		this.array.push( load_element );
	}
}
}

class cookie_load_element {
constructor(
	name,
	datum )
{
	this.name = name;
	this.daum = datum;
	this.datum_unescape = unescape( datum );
}
}
