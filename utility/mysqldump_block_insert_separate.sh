:
# ---------------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_block_insert_separate.sh
# ---------------------------------------------------------------------

grep -i 'insert into'			|
sed 's/),/),\n/g'			|
#grep -v "'0000-00-00'"			|
#grep -v "'0000-00-00 00:00:00'"		|
cat

exit 0
