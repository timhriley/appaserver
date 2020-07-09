{
char msg[ 65536 ];
sprintf( msg, "\n%s/%s()/%d\n",
__FILE__,
__FUNCTION__,
__LINE__ );
m2( "hydrology", msg );
}
