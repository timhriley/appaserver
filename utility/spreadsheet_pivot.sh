:
# $APPASERVER_HOME/utility/spreadsheet_pivot.sh
# ----------------------------------------------
# Freely available software. See appaserver.org
# ----------------------------------------------

grep -v '^$'					|
grep -v '^#'					|
piece_quote_comma "" '^'			|
label_dictionary.e '^'				|
cat

exit 0
