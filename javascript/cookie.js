// $APPASERVER_HOME/javascript/cookie.js
// ---------------------------------------------------------------
// This was extracted and modified
// from "JavaScript The Definitive Guide"
// by David Flanagan, published
// by O'Reilly, 1998.
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

// COOKIE constructor
// ------------------
function cookie( document, cookie_key, hours, path, domain, samesite, secure )
{
	this.$document = document;

	this.$cookie_key = cookie_key;

	if ( hours )
		this.$expiration =
			new Date( ( new Date()).getTime() + hours * 3600000 );
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

	if ( samesite )
		this.$samesite = samesite;
	else
		this.$samesite = null;
}

cookie.prototype.store = function ()
{
	var cookie_value = "";
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

	var cookie = this.$cookie_key + '=' + cookie_value;

	if ( this.$expiration )
		cookie += '; expires=' + this.$expiration.toGMTString();

	if ( this.$path )
		cookie += '; path=' + this.$path;

	if ( this.$domain )
		cookie += '; domain=' + this.$domain;

	if ( this.$secure )
		cookie += '; secure=' + this.$secure;

	if ( this.$samesite )
		cookie += '; SameSite=' + this.$samesite;

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

	var start = all_cookies.indexOf( this.$cookie_key + '=' );

	if ( start == -1 ) return false;

	start += this.$cookie_key.length + 1;

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
	var cookie = this.$cookie_key + '=';

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

