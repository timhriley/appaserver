:
# $APPASERVER_HOME/utility/mysqldump_password_file.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit Appaserver.org
# ---------------------------------------------------------------

mysqldump_password_file=/tmp/mysql_password_file_$$.config

echo "" > $mysqldump_password_file
chmod go-rw $mysqldump_password_file
echo $mysqldump_password_file

exit 0
