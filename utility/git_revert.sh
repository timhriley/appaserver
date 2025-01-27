:
# $APPASERVER_HOME/utility/git_revert.sh
# --------------------------------------
git status				|
grep 'modified: '			|
column.e 1				|
xargs.e git checkout -- {}		|
cat

exit 0
