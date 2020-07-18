:
# $APPSERVER_HOME/utility/purge_old_filenames.sh
# ----------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 directory days_older_than" 1>&2
	exit 1
fi

directory=$1
days_older_than=$2

cd $directory

if [ $? -ne 0 ]
then
	echo "$0 exiting early." 1>&2
	exit 1
fi

ls -1 					|
filename_date_older.e $days_older_than	|
xargs.e 'rm -f {}'			|
cat

exit 0
