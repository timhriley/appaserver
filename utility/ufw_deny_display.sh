:
# --------------------------------------------
# $APPASERVER_HOME/utility/ufw_deny_display.sh
# --------------------------------------------

sudo ufw status					|
grep DENY					|
column.e 2					|
sort_delimited_numbers.e '.' n

exit 0
