:
# $APPASERVER_HOME/utility/git_commit.sh
# --------------------------------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 \"message\"" 1>&2
	exit 1
fi

message="$1"

cd $APPASERVER_HOME
git commit -am "$message"

exit $?
