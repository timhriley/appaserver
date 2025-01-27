{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d\n",
__FILE__,
__FUNCTION__,
__LINE__ );
msg( (char *)0, message );
}
