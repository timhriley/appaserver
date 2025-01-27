:
# $APPASERVER_HOME/utility/git_pull_origin.sh
# -------------------------------------------
git checkout development
git pull origin
git checkout main
git pull origin
git checkout development

exit 0
