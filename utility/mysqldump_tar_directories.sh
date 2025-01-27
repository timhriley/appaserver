:
# -----------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_tar_directories.sh
# -----------------------------------------------------
# Retired
# -----------------------------------------------------

cd /var/backups/

ls -1 appaserver/mysqldump_*.config			|
piece.e '.' 0						|
sed 's|appaserver/mysqldump_||'				|
sed 's|^|appaserver/|'					|
cat

exit 0
