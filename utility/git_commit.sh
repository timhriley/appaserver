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
make clean >/dev/null
git commit -am "$message"
make >/dev/null

exit 0
