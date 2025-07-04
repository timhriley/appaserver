:
# $APPASERVER_HOME/src_predictive/export_home_edition.sh
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
	echo "Error in `basename.e $0 n`: you need to . set-database home" 1>&2
	exit 1
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
directory="`appaserver_data_directory`/$application"
date=`now.sh ymd`
input_export_subschema="${directory}/export_subschema_${application}_${date}.sh"
input_export_entity="${directory}/export_table_entity_${date}.sh"
input_export_vendor="${directory}/export_table_vendor_${date}.sh"
input_export_financial_institution="${directory}/export_table_financial_institution_${date}.sh"
output_export_process_role="/tmp/export_process_role_$$.sql.gz"

export_subschema '' "$tablelist_delimited" n >/dev/null
export_table '' entity shell_script >/dev/null
export_table '' vendor shell_script >/dev/null
export_table '' financial_institution shell_script >/dev/null
export_process_role.sh supervisor $output_export_process_role

(
zcat $input_mysqldump
zcat $output_export_process_role
grep '^insert' < $input_export_subschema
grep '^insert' < $input_export_entity
grep '^insert' < $input_export_vendor
grep '^insert' < $input_export_financial_institution
) |
gzip > $deploy_filename

rm	$input_mysqldump			\
	$input_export_subschema			\
	$input_export_entity			\
	$input_export_vendor			\
	$input_export_financial_institution	\
	$output_export_process_role

echo "Created: $deploy_filename"

exit 0
