/* c:\roommate\source\qpl\lib_dvr.c */
/* -------------------------------- */

#include "lib.h"

main()
{
	test_skip_word();
}

test_skip_word()
{
	printf( "Should be (OK tim) is (%s)\n",
		skip_words( "   should skip this OK tim", 3 ) );
}
