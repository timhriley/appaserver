:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_block_insert_separate.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

grep -i 'insert into'			|
sed 's/),/),\n/g'			|
cat

exit 0
