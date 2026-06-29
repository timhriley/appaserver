:
# $APPASERVER_HOME/utility/git_merge_main.sh
# ------------------------------------------

current_branch=$(git branch | grep '^*' | column.e 1)

if [ "$current_branch" = "main" ]
then
	echo "ERROR in `basename.e $0 n`: You are on branch main." 1>&2
	exit 1
fi

git checkout main

if [ $? -ne 0 ]
then
	exit 1
fi

git merge $current_branch
return_value=$?

git checkout $current_branch

if [ $return_value -eq 0 ]
then
	echo "Branch main is now caught up with this branch."
fi

exit 0
