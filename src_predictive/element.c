/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "boolean.h"
#include "account.h"
#include "element.h"

char *element_select( void )
{
	return "element,accumulate_debit_yn";
}

ELEMENT *element_parse(	char *input )
{
	char element_name[ 128 ];
	char piece_buffer[ 16 ];
	ELEMENT *element;

	if ( !input ) return (ELEMENT *)0;

	piece( element_name, SQL_DELIMITER, input, 0 );
	element = element_new( strdup( element_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	element->accumulate_debit = ( *piece_buffer == 'y' );

	return element;
}

ELEMENT *element_fetch( char *element_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf(where,
		"element = '%s'	",
		element_name );

	sprintf(sys_string,
		"echo \"select %s from %s where %s;\" | sql.e",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_select(),
		"element",
		where );

	return element_parse( pipe2string( sys_string ) );
}

ELEMENT *element_account_name_fetch(
			char *account_name )
{
	char sys_string[ 1024 ];
	char *from;
	char join[ 256 ];
	char where[ 256 ];

	from = "account,subclassification,element";

	sprintf(join,
	"account.subclassification = subclassification.subclassification and "
	"subclassification.element = element.element " );

	sprintf(where,
		"account = '%s' and			"
		"%s					",
		account_escape_name( account_name ),
		join );

	sprintf(sys_string,
		"echo \"select %s from %s where %s;\" | sql.e",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_select(),
		from,
		where );

	return element_parse( pipe2string( sys_string ) );
}

ELEMENT *element_new( char *element_name )
{
	ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( ELEMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	element->element_name = element_name;

	element->accumulate_debit =
		element_accumulate_debit(
			element->element_name );

	return element;
}

boolean element_accumulate_debit(
			char *element_name )
{
	ELEMENT *element;

	if ( ! ( element =
			element_fetch(
				element_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot fetch element for (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 element_name );
		return 0;
	}

	return element->accumulate_debit;
}
