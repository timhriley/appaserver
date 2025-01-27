:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_all.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

backup_directory=`appaserver_backup_directory`

if [ ! -d "${backup_directory}" ]
then
	echo "ERROR in `basename.e $0 n`: cannot get backup_directory from configuration file" 1>&2
	exit 1
fi

(
ls -1 $backup_directory/*.config |
while read parameter_file
do
	if [ -r "$parameter_file" ]
	then
		output_tag=`basename.e $parameter_file y n`
		log_file="${backup_directory}/${output_tag}.log"

		echo "mysqldump_appaserver $parameter_file 2>>$log_file" > $log_file
		mysqldump_appaserver $parameter_file 2>>$log_file
	fi
done
) | mysqldump_summarize.sh

appaserver_trim_log.sh

exit 0
