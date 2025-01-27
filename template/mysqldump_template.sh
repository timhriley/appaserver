:
# $APPASERVER_HOME/template/mysqldump_template.sh
# -----------------------------------------------

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

if [ "$application" != "template" ]
then
	echo "Error in $0: you must connect to the template database." 1>&2
	exit 1
fi

input_filename=`mysqldump.sh y | column.e 1`
output_filename="$APPASERVER_HOME/template/mysqldump_template.sql.gz"

(
zcat $input_filename					|
grep -vi 'insert into `appaserver_user`'		|
grep -vi 'insert into `login_default_role`'		|
grep -vi 'insert into `session`'			|
grep -vi 'insert into `select_statement`'		|
grep -vi 'insert into `role_appaserver_user`'
)							|
gzip > $output_filename

rm $input_filename
echo "Created: $output_filename"

exit 0
