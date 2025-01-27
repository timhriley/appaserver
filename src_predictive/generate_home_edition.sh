:
# $APPASERVER_HOME/src_predictive/generate_home_edition.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

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

if [ "$application" != "home" ]
then
	application="home"
fi

if [ $# -ne 1 ]
then
	echo "Usage: `basename.e $0 n` tablelist_file" 1>&2
	exit 1
fi

tablelist_file=$1

# Synchronize with IMPORT_PREDICT_SQLGZ
# in $APPASERVER_HOME/src_predictive/import_predict.h
# --------------------------------------------------- 
deploy_filename="$APPASERVER_HOME/src_predictive/home_edition.sql.gz"

tablelist_line=`cat $tablelist_file`
tablelist_delimited=`cat $tablelist_file | joinlines.e '^'`
input_mysqldump=`mysqldump_quick.sh $application "$tablelist_line" | column.e 1`
directory=`appaserver_data_directory`
date=`now.sh ymd`
input_export_subschema="${directory}/export_subschema_${application}_${date}.sh"
input_export_entity="${directory}/export_table_entity_${date}.sh"
output_export_process_role="/tmp/export_process_role_$$.sql.gz"
output_export_process_predict="/tmp/export_process_predict_$$.sql"

export_subschema '' "$tablelist_delimited" n >/dev/null
export_table '' entity shell_script >/dev/null
export_process_role.sh supervisor $output_export_process_role
export_process_predict.sh $output_export_process_predict

(
zcat $input_mysqldump
zcat $output_export_process_role
cat $output_export_process_predict
grep '^insert' < $input_export_subschema
grep '^insert' < $input_export_entity
) |
gzip > $deploy_filename

rm	$input_mysqldump		\
	$input_export_subschema		\
	$input_export_entity		\
	$output_export_process_role	\
	$output_export_process_predict

echo "Created: $deploy_filename"

exit 0
