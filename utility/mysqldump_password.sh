:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_password.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit Appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_database first." 1>&2
	exit 1
fi

cat /etc/appaserver.config	|
grep '^password='		|
piece.e '=' 1

exit 0
