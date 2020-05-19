:
# ----------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/application_backup_list.sh
# ----------------------------------------------------------

appaserver_config_file="/etc/appaserver.config"

# Synchronize with MYSQLDUMP_FORK_BACKUP_DIRECTORY
# ------------------------------------------------
backup_directory="/var/backups/appaserver"

ls -1 $backup_directory/mysqldump_*.config			|
while read parameter_file
do
	if [ -r "$parameter_file" ]
	then
		application=`basename.e $parameter_file y n	|\
			     sed 's/mysqldump_//'`
		echo $application
	fi
done

exit 0
