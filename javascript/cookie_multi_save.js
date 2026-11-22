// $APPASERVER_HOME/javascript/cookie_multi_save.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class cookie_multi_save {
constructor(
	cookie_key,
	samesite_string,
	secure_string,
	recall_equal_delimiter,
	recall_multi_delimiter,
	cookie_multi_save_element_array )
{
	let multi_save_element_string =
		cookie_multi_save_element_string(
			cookie_key,
			recall_equal_delimiter,
			recall_multi_delimiter,
			cookie_multi_save_element_array );

	this.cookie_save =
		new cookie_save(
			samesite_string,
			secure_string,
			multi_save_element_string
				/* recall_save_element_string */ );
}
}

function cookie_multi_save_element_string(
	cookie_key,
	recall_equal_delimiter,
	recall_multi_delimiter,
	cookie_multi_save_element_array )
{
	let string = cookie_key + '=';

	for (	const cookie_multi_save_element of
		cookie_multi_save_element_array )
	{
		string +=
			cookie_multi_save_element.name +
			recall_equal_delimiter;

		let first_time = true;

		for (	const cookie_multi_save_datum of
			cookie_multi_save_element.
				cookie_multi_save_datum_array )
		{
			if ( first_time )
				first_time = false;
			else
				string += recall_multi_delimiter;

			string += cookie_multi_save_datum.datum;
		}
	}

	return string;
}

class cookie_multi_save_element_array {
constructor(
	recall_equal_delimiter,
	recall_block_delimiter,
	recall_multi_delimiter,
	recall_multi_index_delimiter,
	multi_element_name_array )
{
	var multi_save_element;
	this.array = new Array();

	for ( const element_name of multi_element_name_array )
	{
		var element =
			document.getElementById(
				element_name );

		/* If the element was removed */
		/* -------------------------- */
		if ( !element ) continue;

		multi_save_element =
			new cookie_multi_save_element(
				recall_multi_index_delimiter,
				element_name,
				element.options
				/* element_option_array */  );

		if ( multi_save_element )
			this.array.push(
				multi_save_element );
	}
}
}

class cookie_multi_save_element {
constructor (
	recall_multi_index_delimiter,
	name,
	element_option_array )
{
	this.name = name;

	this.cookie_multi_save_datum_array =
		cookie_multi_save_datum_array(
			recall_multi_index_delimiter,
			element_option_array );

	if ( !this.cookie_multi_save_datum_array.length ) return false;
}
}

function cookie_multi_save_datum_array(
	recall_multi_index_delimiter,
	element_option_array )
{
	var multi_save_datum;
	let array = new Array();

	for ( const option of element_option_array )
	{
		if ( option.selected )
		{
			let value_array =
				option.value.split(
					recall_multi_index_delimiter );

			multi_save_datum =
				new cookie_multi_save_datum(
					value_array[ 0 ] /* datum */,
					value_array[ 1 ] /* offset */ );

			array.push( multi_save_datum );
		}
	}

	return array;
}

class cookie_multi_save_datum {
constructor (
	datum,
	offset )
{
	this.datum = datum;
	this.offset = offset;
}
}

