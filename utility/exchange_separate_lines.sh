:
# ---------------------------------------------------------------
# $APPASERVER_HOME/utility/exchange_separate_lines.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

sed 's|><|>\n<|g'			|
sed 's|>\(.*$\)|>\n\1|g'		|
sed 's|</\(.*$\)|\n</\1|g'		|
cat

exit 0
