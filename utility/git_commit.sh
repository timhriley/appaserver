:
# $APPASERVER_HOME/utility/git_commit.sh
# --------------------------------------
if [ "$#" -lt 1 ]
then
	echo "Usage: $0 \"message\" [recomile_yn]" 1>&2
	exit 1
fi

message="$1"

if [ "$#" -eq 2 ]
then
	recompile_yn="$2"
else
	recompile_yn="y"
fi

cd $APPASERVER_HOME
make clean >/dev/null
git commit -am "$message"

if [ "$recompile_yn" = 'y' ]
then
	make >/dev/null
fi

exit $?
