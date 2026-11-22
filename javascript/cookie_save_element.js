// $APPASERVER_HOME/javascript/cookie_save_element.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class cookie_save_element_array {
constructor(
	element_name_array )
{
	var datum;
	var save_element;

	this.array = new Array();

	for ( const name of element_name_array )
	{
		let element =
			document.getElementById(
				name );

		if (	element
		&&	element.value.length
		&&	element.value != "select" )
		{
			datum =
				cookie_save_element_array_datum(
					element.type,
					element.value,
					element.checked );

			if ( datum )
			{
				save_element =
					new cookie_save_element(
						name,
						datum );

				this.array.push( save_element );
			}
		}
	}
}
}

class cookie_save_element {
constructor(
	name,
	datum )
{
	this.name = name;
	this.daum = datum;
	this.datum_escape = escape( datum );
}
}

function cookie_save_element_array_datum(
	element_type,
	element_value,
	element_checked )
{
	var datum = false;

	if ( element_type == 'checkbox' )
	{
		if ( element_checked ) datum = 'Y';
	}
	else
	{
		datum = element_value;
	}

	return datum;
}
