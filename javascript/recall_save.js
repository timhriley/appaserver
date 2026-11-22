// $APPASERVER_HOME/javascript/recall_save.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class recall_save {
constructor(
	cookie_key,
	samesite_string,
	secure_string,
	element_name_delimited_string,
	recall_name_delimiter /* Probably ~ */,
	recall_equal_delimiter /* Probably == */,
	recall_block_delimiter /* Probably ~~ */ )
{
	let element_name_array =
		element_name_delimited_string.split(
			recall_name_delimiter );

	this.cookie_save_element_array =
		new cookie_save_element_array(
			element_name_array,
			recall_equal_delimiter,
			recall_block_delimiter );

	if ( this.cookie_save_element_array.array.length )
	{
		let save_element_string =
			recall_save_element_string(
				cookie_key,
				this.cookie_save_element_array.array
					/* cookie_save_element_array */,
				recall_equal_delimiter,
				recall_block_delimiter );
		this.cookie_save =
			new cookie_save(
				samesite_string,
				secure_string,
				save_element_string );
	
		document.cookie = this.cookie_save.cookie_string
	}
}
}

function recall_save_element_string(
	cookie_key,
	cookie_save_element_array,
	recall_equal_delimiter,
	recall_block_delimiter )
{
	var cookie_save_element;
	let first_time_boolean = true;
	let string = cookie_key + '=';

	for ( cookie_save_element of cookie_save_element_array )
	{
		if ( first_time_boolean )
			first_time_boolean = false;
		else
			string += recall_block_delimiter;

		string +=
			cookie_save_element.name +
			recall_equal_delimiter +
			cookie_save_element.datum_escape;
	}

	return string;
}
