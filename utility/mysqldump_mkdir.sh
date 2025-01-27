:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_mkdir.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit Appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 database backup_directory" 1>&2
	exit 1
fi

database=$1
backup_directory=$2

cd $backup_directory

if [ "$?" -ne 0 ]
then
	echo "ERROR in $0: cd $backup_directory failed" 1>&2
	exit 1
fi

if [ -d $database ]
then
	exit 0
fi

mkdir $database

if [ "$?" -ne 0 ]
then
	echo "ERROR in $0: mkdir $database failed" 1>&2
	exit 1
fi

chmod g+wxs $database
chmod o-rwx $database

cd $database

mkdir son father grandfather greatgrandfather greatgreatgrandfather
chmod g+wxs son father grandfather greatgrandfather greatgreatgrandfather
chmod o-rwx son father grandfather greatgrandfather greatgreatgrandfather

exit 0

