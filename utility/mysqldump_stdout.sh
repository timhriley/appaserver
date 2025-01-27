:
# $APPASERVER_HOME/utility/mysqldump_stdout.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 database" 1>&2
	exit 1
fi

database=$1

mysqldump_mysql_user=`mysqldump_mysql_user.sh`
mysqldump_password=`mysqldump_password.sh`
mysqldump_password_file=`mysqldump_password_file.sh`

echo "[mysqldump]" > $mysqldump_password_file
echo "password=${mysqldump_password}" >> $mysqldump_password_file

options="--defaults-extra-file=${mysqldump_password_file} -u $mysqldump_mysql_user -c --extended-insert=FALSE --force --quick --add-drop-table"

nice mysqldump $options $database	|
mysqldump_sed.sh

rm ${mysqldump_password_file}

exit 0
