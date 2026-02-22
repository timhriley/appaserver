:
. set-database template

cd `appaserver_log_directory`

if [ $? -ne 0 ]
then
	echo "Error: cd failed" 1>&2
	exit 1
fi

find . -empty				|
sed 's|\./appaserver_||'		|
piece.e '.' 0				|
xargs.e 'delete_application "" {} y'

exit 0
