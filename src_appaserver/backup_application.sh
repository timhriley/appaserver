:
if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_database first." 1>&2
	exit 1
fi

application_database=$APPASERVER_DATABASE

if [ "$#" -lt 2 ]
then
	echo "Usage:$0 ignored days_to_keep [backup_subdirectory]" 1>&2
	exit 1
fi

days_to_keep=$2

if [ "$#" -eq 3 ]
then
	backup_subdirectory=$3
else
	backup_subdirectory="backup"
fi

# Returns 0 if the first character is a slash.
# --------------------------------------------
absolute_path=`strncmp.e "$backup_subdirectory" "/" 1`

if [ "$absolute_path" -eq 0 ]
then
	backup_path=$backup_subdirectory
else
	backup_path=$APPASERVER_HOME/$backup_subdirectory
fi

now=`date.e 0 | sed 's/:/-/'`
application=$(echo $application_database | sed 's/:.*$//g')

# Backup Appaserver Tables
# ------------------------
output_file=$APPASERVER_HOME/data/insert_${application}_appaserver.sql.gz
backup_file=${backup_path}/insert_${application}_appaserver-${now}.sql.gz

export_application $application_database '' '' '' $application n y '' '' mysql sql_statements > /dev/null

mv ${output_file} $backup_file

# Backup Application Tables
# -------------------------
output_file=$APPASERVER_HOME/data/insert_${application}.sql.gz
backup_file=${backup_path}/insert_${application}-${now}.sql.gz

export_application $application_database '' '' '' $application n n '' '' mysql sql_statements > /dev/null

mv ${output_file} $backup_file

# Execute create application appaserver
# -------------------------------------
output_file=$APPASERVER_HOME/data/create_${application}_appaserver.sh
backup_file_appaserver=${backup_path}/create_${application}_appaserver-${now}.sh

create_application $application_database y '' '' $application y y really_yn '' >/dev/null

cat ${output_file} | sed 's/^exit 0//' > $backup_file_appaserver

# Execute create application
# --------------------------
output_file=$APPASERVER_HOME/data/create_${application}.sh
backup_file=${backup_path}/create_${application}-${now}.sh

create_application $application_database y '' '' $application n y really_yn '' >/dev/null

cat ${output_file} | sed 's/^exit 0//' > $backup_file

# Clean out backup files
# ----------------------
find ${backup_path}/ -type f -mtime +$days_to_keep -exec rm {} \;

exit 0
