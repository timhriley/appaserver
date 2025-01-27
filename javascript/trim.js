// $APPASERVER_HOME/javascript/trim.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function trim_character( source_string, character )
{
	var length;
	var return_string;
	var ch;

	length = source_string.length;

	if ( !length ) return source_string;

	return_string = "";

	for ( var i = 0; i < length; i++ )
	{
		ch = source_string.charAt( i );

		if ( ch == character ) continue;

		return_string = return_string + ch;
	}

	return return_string;
}

function trim_trailing_piece( source_string, delimiter )
{
	var length;

	length = source_string.length;

	if ( !length ) return source_string;

	for ( var i = length - 1; i; i-- )
	{
		if ( source_string.charAt( i ) == delimiter )
		{
			return source_string.substring( 0, i );
		}
	}

	return source_string;
}

function left_trim( s )
{
	var return_string = "";
	var i;

	for ( i = 0; i < s.length; i++ )
		if ( s.charCodeAt( i ) != 32 )
			return_string += s.charAt( i );

	return return_string;
}

function right_trim( s )
{
	var i, end;

	if ( s.length == 0 ) return s;

	for ( i = s.length - 1; i; i-- )
	{
		if ( s.charCodeAt( i ) != 32
		&&   s.charCodeAt( i ) != 47 )
		{
			break;
		}
	}
	if ( i == s.length - 1 )
	{
		return s;
	}
	else
	{
		return s.substr( 0, i + 1 );
	}
}

