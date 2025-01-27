:
# $APPASERVER_HOME/src_system/graphviz_database_schema_process.sh
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

document_root=$(document_root)
data_directory="`appaserver_data_directory`/$application"
output_directory=$data_directory

echo "$0" "$*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 process appaserver_yn" 1>&2
	exit 1
fi

process_name=$1                       	# Assumed letters_and_underbars
appaserver_yn=$2

process_title=`echo "$process_name" | format_initial_capital.e`

echo "<html><head>"
echo "<link rel=stylesheet type=text/css href=/appaserver/template/style.css>"
echo "<SCRIPT language=JavaScript1.2 src="/appaserver_home/javascript/timlib.js"></SCRIPT>"
echo "</head>"
echo "<body onload=timlib_wait_over()>"
echo "<h1>$process_title</h1>"
echo "<h2>`date.sh`</h2>"

# Output
# ------
cd $output_directory

graphviz_database_schema.sh "$process_name" "$appaserver_yn"

echo "<p>Best when viewed in <a href='https://en.wikipedia.org/wiki/Evince' target=_new>Evince</a>"
echo "</body></html>"

exit 0
