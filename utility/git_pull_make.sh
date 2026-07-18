:
# $APPASERVER_HOME/utility/git_pull_make.sh
# -----------------------------------------
cd $APPASERVER_HOME
git checkout main
git pull origin
make touch >/dev/null && make >/dev/null && appaserver-upgrade

exit 0
