:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_table.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_database first." 1>&2
	exit 1
fi

if [ $# -lt 2 ]
then
	echo "Usage: `basename.e $0 n` output_sql table [...]" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

outputfile=$1
shift 

mysqldump_mysql_user=`mysqldump_mysql_user.sh`
mysqldump_password=`mysqldump_password.sh`
mysqldump_password_file=`mysqldump_password_file.sh`

echo "[mysqldump]" > $mysqldump_password_file
echo "password=${mysqldump_password}" >> $mysqldump_password_file

options="--defaults-extra-file=$mysqldump_password_file -u $mysqldump_mysql_user -c --skip-extended-insert --force --quick --add-drop-table"

nice mysqldump $options $application $*		|
mysqldump_sed.sh				|
cat > $outputfile

rm $mysqldump_password_file

exit 0
