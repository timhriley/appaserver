// $APPASERVER_HOME/javascript/cookie_load_raw.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function cookie_load_raw_array(
	cookie_string,
	cookie_load_raw_delimiter /* Probably ; */ )
{
	var load_raw;

	// Note: there's a space after each ';'
	let cookie_raw_string_array =
		cookie_string.split(
			cookie_load_raw_delimiter );

	let array = new Array();

	for ( cookie_raw_string of cookie_raw_string_array )
	{
		load_raw = new cookie_load_raw( cookie_raw_string );

		if ( !load_raw.cookie_key ) return null;

		array.push( load_raw );
	}

	return array;
}

class cookie_load_raw {
constructor(
	cookie_raw_string )
{
	let cookie_raw_string_trim = cookie_raw_string.trimStart();
	let equal_position = cookie_raw_string_trim.indexOf( '=' );

	this.cookie_key =
		cookie_raw_string_trim.
			substring( 0, equal_position );

	this.cookie_string =
		cookie_raw_string_trim.
			substring( equal_position + 1 );
}
}

function cookie_load_raw_key_string(
	cookie_key,
	cookie_load_raw_array )
{
	for ( const cookie_load_raw of cookie_load_raw_array )
	{
		if ( cookie_load_raw.cookie_key == cookie_key )
		{
			return cookie_load_raw.cookie_string;
		}
	}

	return false;
}
