:
# ------------------------------------------------------------------------
# $APPASERVER_HOME/code_count.sh
# ------------------------------------------------------------------------
# This file represents 15 lines of code. However, this isn't adding to the
# line or file counts because the find doesn't include shell scripts.
# Shell scripts are excluded because many are generated and don't
# represent typing.
# ------------------------------------------------------------------------

/bin/echo -n "File count: "
(
for file in `find . -name '*.c' -o -name '*.h' -o -name makefile -o -name '*.js' | grep -v '/upgrade/'`
do
	echo $file
done
) | wc -l | commas_in_long.e

/bin/echo -n "Line count: "
(
for file in `find . -name '*.c' -o -name '*.h' -o -name makefile -o -name '*.js' | grep -v '/upgrade/'`
do
	cat $file
done
# ------------------------------
# Skip empty lines and comments.
# ------------------------------
) | grep -v '^$' | grep -v '^\/\*' | wc -l | commas_in_long.e

exit 0
