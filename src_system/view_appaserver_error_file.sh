:
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_system/view_appaserver_error_file.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------


if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: APPASERVER_DATABASE is not defined." 1>&2
	exit 1
fi

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 process line_count" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

process=$1
line_count=$2

directory=`appaserver_error_directory`

if [ "$line_count" = "" -o "$line_count" = "line_count" ]
then
	line_count=50
fi

filename=$directory/appaserver_${application}.err

if [ ! -r $filename ]
then
	filename=$directory/appaserver/appaserver_${application}.err
fi

process_display=`echo $process | string_initial_capital.e ',' 0`

(
echo "<h1>$process_display</h1>"
echo  "<table>"
tail -$line_count $filename				|
sed 's/^/<tr><td>/'
echo "</table>"
)							|
html_wrapper.e

exit 0
