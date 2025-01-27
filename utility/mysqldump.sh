:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_database first." 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -lt 1 ]
then
	echo "Usage: `basename.e $0 n` each_line_insert_yn [tables]" 1>&2
	exit 1
fi

each_line_insert_yn="$1"

mysqldump_mysql_user=`mysqldump_mysql_user.sh`
mysqldump_password=`mysqldump_password.sh`
mysqldump_password_file=`mysqldump_password_file.sh`

echo "[mysqldump]" > $mysqldump_password_file
echo "password=${mysqldump_password}" >> $mysqldump_password_file

#		--lock-tables					\

all_options="	--defaults-extra-file=$mysqldump_password_file	\
		-u $mysqldump_mysql_user			\
		-c						\
		--force						\
		--disable-keys					\
		--quick						\
		--lock-tables					\
		--add-drop-table"

if [ "$each_line_insert_yn" = "y" ]
then
	options="$all_options --skip-extended-insert"
else
	options="$all_options"
fi

outputfile=/tmp/mysqldump_${application}_`date.e 0 | tr ':' '-'`.sql.gz

shift

nice mysqldump $options $application $*			|
mysqldump_sed.sh					|
gzip 							|
cat > $outputfile

rm $mysqldump_password_file

echo "Created $outputfile"

exit 0
