:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/sed_remove_leading_spaces.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

sed 's/^[ \t]*//g'			|
cat

exit 0
