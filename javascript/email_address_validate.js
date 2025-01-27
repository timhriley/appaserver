// $APPASERVER_HOME/javascript/email_address_validate.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------
function validateEmail( form )
{

	 var regular_expression = /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
	if ( !form.email_address.value.match( regular_expression )) {
		alert("Invalid email address");
		form.email_address.focus();
		form.email_address.select();
		return false;
	}
	return true;
}

