/* make_date_time_between_compatable.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <ctype.h>
#include "make_date_time_between_compatable.h"

void make_date_time_between_compatable(		char **to_date,
						char **from_time,
						char **to_time,
						char *from_date )
{
	if ( !**to_date
	||   !isdigit( **to_date ) )
	{
		*to_date = from_date;
	}

	if ( !**from_time
	||   !isdigit( **from_time ) )
	{
		*from_time = "0000";
	}

	if ( !**to_time
	||   !isdigit( **to_time ) )
	{
		*to_time = "2359";
	}
}

