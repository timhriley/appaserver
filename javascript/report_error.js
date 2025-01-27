//
// report_error.js
// ------------------------------------------------------------------
//
// Arranged from "JavaScript, The Definitive Guide" by David Flanagan
// ------------------------------------------------------------------

var error_count = 0;
var email = "timriley@timriley.net"

function report_error( msg, url, line )
{
	var w = window.open( 	"", 
				"error" + error_count++,
				"resizeable,status,width=725,height=400" );
	var d = w.document;

	d.writeln( '<DIV align=center>' );
	d.writeln( '<font size=7 face="helvetica"><b>' );
	d.writeln( 'OOPS ... A JavaScript Error Has Occurred!' );
	d.writeln( '</b></font><br><hr size=4 width="80%">' );
	d.writeln( '<form action="mailto:' + email + '" method=post' );
	d.writeln( 'enctype="text/plain">' );
	d.writeln( '<font size=3>' );
	d.writeln( '<i>Click the "Report Error" button to send a bug report.' );
	d.writeln( '</i><br>' );
	d.writeln( '<input type="submit" value="Report Error">&nbsp;&nbsp;' );
	d.writeln( '<input type="button" value="Dismiss"' );
	d.writeln( 'onClick="self.close()">' );
	d.writeln( '</div>' );
	d.writeln( '<div align=right>' );
	d.writeln( '<br><br>Your name <i>(optional)</i>: ' );
	d.writeln( '<input size=42 name="name" value="">' );
	d.writeln( '<br>Error Message: ' );
	d.writeln( '<input size=42 name="message" value="' + msg + '">' );
	d.writeln( '<br>Document: <input size=42 name="url"' );
	d.writeln( 'value="' + url + '">' );
	d.writeln( '<br>Line Number: <input size=42 name="line"' );
	d.writeln( 'value="' + line +'">' );
	d.writeln( '<br>Browser Version: ' );
	d.writeln( '<input size=42 name="version"' );
	d.writeln( 'value="' + navigator.userAgent + '">' );
	d.writeln( '</div></font>' );
	d.writeln( '</form>' );

	d.close();

	return true;
} // function report_error()

self.onerror=report_error

