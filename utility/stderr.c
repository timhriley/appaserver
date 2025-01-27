/*
fprintf(
stderr,
"%s/%s()/%d:\n",
__FILE__,
__FUNCTION__,
__LINE__ );
*/
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}

