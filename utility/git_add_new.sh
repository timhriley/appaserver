:
# $APPASERVER_HOME/utility/git_add_new.sh
# ---------------------------------------
git status				|
trim.e					|
grep -v ' '				|
grep -v '^$'				|
xargs.e git add {}			|
cat

exit 0
