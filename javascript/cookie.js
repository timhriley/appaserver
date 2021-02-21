// javascript/cookie.js
// --------------------------------------
// This was extracted and modified
// from "JavaScript The Definitive Guide"
// by David Flanagan, published
// by O'Rielly, 1998.
// --------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

// COOKIE constructor
// ------------------
function cookie( document, name, hours, path, domain, secure )
{
	this.$document = document;
	this.$name = name;

	if ( hours )
		this.$expiration = new Date( ( new Date()).getTime() + 
						hours * 3600000 );
	else
		this.$expiration = null;

	if ( path )
		this.$path = path;
	else
		this.$path = null;

	if ( domain )
		this.$domain = domain;
	else
		this.$domain = null;

	if ( secure )
		this.$secure = secure;
	else
		this.$secure = null;
}

cookie.prototype.store = function ()
{
	var cookie_value = "";
	// var escaped_value;
	var store_value;

	for ( var property in this )
	{
		if ( ( property.charAt( 0 ) == '$' )
		|| ( ( typeof this[ property ] ) == 'function' ) 
		||   ( property.toString() == '' ) )
		{
			continue;
		}


		if ( this[ property ] != '' )
		{
			store_value = this[ property ];
			if ( cookie_value != "" ) cookie_value += '&';
			cookie_value += property + '!*' + store_value;
		}
	}

	var cookie = this.$name + '=' + cookie_value;

	if ( this.$expiration )
		cookie += '; expires=' + this.$expiration.toGMTString();
	if ( this.$path )
		cookie += '; path=' + this.$path;
	if ( this.$domain )
		cookie += '; domain=' + this.$domain;
	if ( this.$secure )
		cookie += '; secure=' + this.$secure;

	cookie += '; SameSite=Strict';

	this.$document.cookie = cookie;
}

cookie.prototype.display = function ()
{
	for ( var property in this )
	{
		if ( ( property.charAt( 0 ) == '$' )
		|| ( ( typeof this[ property ] ) == 'function' ) )
		{
			continue;
		}

		alert(	'cookie.display property ('	+
			property			+
			') = '				+
			this[ property ] );
	}
}

cookie.prototype.load = function ()
{
	var cookie_value;
	var all_cookies = this.$document.cookie;

	if ( all_cookies == "" ) return false;

	var start = all_cookies.indexOf( this.$name + '=' );

	if ( start == -1 ) return false;

	start += this.$name.length + 1;

	var end = all_cookies.indexOf( ';', start );

	if ( end == -1 ) end = all_cookies.length;

	var cookie_evaluate = all_cookies.substring( start, end );

	var a = cookie_evaluate.split( '&' );

	for( var i = 0; i < a.length; i++ )
		a[ i ] = a[ i ].split( '!*' );

	for ( var i = 0; i < a.length; i++ )
	{
		cookie_value = a[ i ][ 1 ];
		this[ a[ i ][ 0 ] ] = cookie_value;
	}

	return true;
}

cookie.prototype.remove = function ()
{
	var cookie = this.$name + '=';

	if ( this.$path ) cookie += '; path=' + this.$path;
	if ( this.$domain ) cookie += '; domain=' + this.$domain;
	cookie += '; expires=Fri, 02-Jan-1970 00:00:00 GMT';
	
	for ( var property in this )
	{
		if ( ( property.charAt( 0 ) == '$' )
		|| ( ( typeof this[ property ] ) == 'function' ) )
		{
			continue;
		}
		this[ property ] = "";
	}
}

