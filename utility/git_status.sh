:
# $APPASERVER_HOME/utility/git_status.sh
# --------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 recompile_yn" 1>&2
	exit 1
fi

recompile_yn=$1

cd $APPASERVER_HOME
make clean >/dev/null
git status

if [ "$recompile_yn" = "y" ]
then
	make >/dev/null
fi

exit 0
