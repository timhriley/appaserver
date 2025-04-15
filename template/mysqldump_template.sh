:
# $APPASERVER_HOME/template/mysqldump_template.sh
# -----------------------------------------------

APPASERVER_DATABASE="template"
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
