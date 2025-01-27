:
# -----------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_appaserver_log.sh
#
# This script archives /var/log/appaserver/appaserver_$database.err
# -----------------------------------------------------------------
# No warranty and freely available software. Visit Appaserver.org
# -----------------------------------------------------------------
if [ "$#" -ne 5 ]
then
	echo "Usage: $0 database log_directory yyyymmdd destination_directory log_extension" 1>&2
	exit 1
fi

database=$1
log_directory=$2
date_yyyymmdd=$3
destination_directory=$4
log_extension=$5

error_file=${log_directory}/appaserver_${database}.${log_extension}
destination_file=${destination_directory}/appaserver_${database}_${log_extension}_${date_yyyymmdd}.gz

gzip -c $error_file > $destination_file

exit 0
