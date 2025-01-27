/* library/maskdvr.c */
/* ----------------- */

#include <stdio.h>
#include <stdlib.h>
#include "mask.h"

int main()
{
/*
	test1();
	test2();
	test3();
*/
	return 0;
}

#ifdef NOT_DEFINED
test3()
{
	char buffer[ 128 ];

	printf( "mask_set() should be (05/98) is (%s)\n",
		mask_set( buffer, "05/98", "##/##" ) );

	printf( "mask_set() should be (05/98) is (%s)\n",
		mask_set( buffer, "5/98", "##/##" ) );

	printf( "mask_set() should be (05/98) is (%s)\n",
		mask_set( buffer, "598", "##/##" ) );

}

test1()
{
	char buffer[ 128 ];

	printf( "mask_set() should be (305-829-0715) is (%s)\n",
		mask_set( buffer, "3058290715", "###-###-####" ) );

	printf( "mask_set() should be (305-829-0715) is (%s)\n",
		mask_set( buffer, "305-829-0715", "###-###-####" ) );

	printf( "mask_set() should be (305-829-0715) is (%s)\n",
		mask_set( buffer, "305.829.0715", "###-###-####" ) );

	printf( "mask_trim() should be (3058290715) is (%s)\n",
		mask_trim( buffer, "305-829-0715" ) );

	printf( "mask_trim() should be (3058290715) is (%s)\n",
		mask_trim( buffer, "3058290715" ) );

}

test2()
{
	char buffer[ 128 ];

	printf( "mask_set() should be (10/97) is (%s)\n",
		mask_set( buffer, "1097", "##/##" ) );

	printf( "mask_set() should be (10/97) is (%s)\n",
		mask_set( buffer, "10/97", "##/##" ) );

	printf( "mask_set() should be (06/97) is (%s)\n",
		mask_set( buffer, "697", "##/##" ) );

}
#endif
