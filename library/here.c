{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d\n",
__FILE__,
__FUNCTION__,
__LINE__ );
m2( application_name, msg );
}
