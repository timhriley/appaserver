:
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/session_delete.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -eq 0 ]
then
	echo \
"Usage: $0 application [...]" 1>&2
	exit 1
fi

while [ $# -gt 0 ]
do
	application=$1

	export APPASERVER_DATABASE=$application
	echo "delete from session;" | sql.e
	shift
done

exit 0
