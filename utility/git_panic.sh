:
# $APPASERVER_HOME/utility/git_panic.sh
# -------------------------------------

git fetch origin
git reset --hard origin/main
git reset --hard HEAD

exit 0
