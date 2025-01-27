:
# ------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_tar_latest_files.sh
# ------------------------------------------------------
# Retired
# -----------------------------------------------------

cd /var/backups/

while read appaserver_directory
do
	# Get the latest date
	# -------------------
	latest_date=`	ls -t $appaserver_directory/son/*_err_*	|\
			head -1					|\
			sed 's/.*_err_//'			|\
			piece.e '.' 0`

	ls -1 $appaserver_directory/son/*${latest_date}*
done

exit 0
