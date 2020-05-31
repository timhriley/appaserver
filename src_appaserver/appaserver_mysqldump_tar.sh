:
# -----------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_mysqldump_tar.sh
# -----------------------------------------------------------

# Synchronize with MYSQLDUMP_FORK_BACKUP_DIRECTORY
# ------------------------------------------------
backup_directory="/var/backups"

output_file=appaserver_var_backups.tar

cd $backup_directory

tar	-c				\
	--exclude="untar/*"		\
	-f $output_file			\
	`mysqldump_tar_directories.sh	|
	 mysqldump_tar_latest_files.sh`

exit $?
