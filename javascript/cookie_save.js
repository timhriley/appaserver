// $APPASERVER_HOME/javascript/cookie_save.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class cookie_save {
constructor(
	samesite_string,
	secure_string,
	recall_save_element_string )
{
	this.cookie_string = recall_save_element_string;

	let nextYear = new Date();

	nextYear.setFullYear(
		nextYear.getFullYear() + 1 );

	this.cookie_string +=
		"; expires=" + nextYear.toGMTString();

	this.cookie_string += "; path=/";

	// this.cookie_string += "; domain=''";

	if ( samesite_string.length )
		this.cookie_string += "; SameSite=" + samesite_string;

	if ( secure_string.length )
		this.cookie_string += "; secure=" + secure_string;

}
}
