:
# --------------------------------------------
# $APPASERVER_HOME/utility/ufw_deny_class_b.sh
# --------------------------------------------

ufw_deny_display.sh	|
grep -v '/'		|
piece.e '.' 0,1,2	|
group.e			|
grep '^255|'		|
piece.e '|' 1

exit 0
