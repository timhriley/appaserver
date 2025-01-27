:
# $APPASERVER_HOME/utility/git_add.sh
# -----------------------------------
git status				|
grep 'modified: '			|
column.e 1				|
xargs.e git add {}			|
cat

exit 0
