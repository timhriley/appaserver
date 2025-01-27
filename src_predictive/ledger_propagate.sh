:
# $APPASERVER_HOME/src_predictive/ledger_propagate.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

echo `basename.e $0 n` "$*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 process_name" 1>&2
	exit 1
fi

process_name=$1

process_title=`echo "$process_name" | string_initial_capital.e '|' 0`

document_body.sh $application
echo "<h1>$process_title</h1>"

ledger_propagate ''

echo "<h3>Process complete.</h3>"
echo "</body></html>"

exit 0
