function display_factorial()
{
	document.write( "<h2>Table of Factorials</h2>" );
	for( i = 1, factorial = 1; i < 10; i++, factorial *= i ) {
		document.write( i + "! = " + factorial );
		document.write( "<br>" );
	}
}
