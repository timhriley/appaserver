:
# $APPASERVER_HOME/utility/purge_binary_logs.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------
echo "show binary logs;"			|
sql.e						|
tail -1						|
piece.e '^' 0					|
xargs.e echo "purge binary logs to \'{}\';"	|
sql.e						|
cat

exit 0
