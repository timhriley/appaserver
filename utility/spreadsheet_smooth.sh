:
# $APPASERVER_HOME/utility/spreadsheet_smooth.sh
# ----------------------------------------------
# Freely available software. See appaserver.org
# ----------------------------------------------

grep -v '^$'					|
grep -v '^#'					|
piece_quote_comma "" '^'			|
label_dictionary.e '^'				|
cat

exit 0
