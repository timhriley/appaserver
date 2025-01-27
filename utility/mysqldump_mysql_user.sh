:
# $APPASERVER_HOME/utility/mysqldump_mysql_user.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit Appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_database first." 1>&2
	exit 1
fi

cat /etc/appaserver.config	|
grep '^mysql_user='		|
piece.e '=' 1

exit 0
