:
# $APPASERVER_HOME/utility/git_merge_current.sh
# ---------------------------------------------

current_branch=$(git branch | grep '^*' | column.e 1)

if [ "$current_branch" = "main" ]
then
	echo "ERROR in `basename.e $0 n`: You are on branch main." 1>&2
	exit 1
fi

if [ $? -ne 0 ]
then
	exit 1
fi

git merge main
return_value=$?

if [ $return_value -eq 0 ]
then
	echo "Branch $current_branch is now caught up with main."
fi

exit 0
