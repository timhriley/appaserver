/*
fprintf(
stderr,
"%s/%s()/%d:\n",
__FILE__,
__FUNCTION__,
__LINE__ );
*/
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

