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
git merge $current_branch
git checkout $current_branch

echo "Branch main is now caught up with this branch."

exit 0
