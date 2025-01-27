:
# describe_each_view.sh
# ---------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 uidpwd" 1>&2
	exit 1
fi

view_list=`echo "select view_name from user_views;" | select.sh $1`

for view in `echo $view_list`
do
	echo $view
	echo "--------------------------------------------"
	describe_table.sh $1 $view | sed 's/.*/	&/'
	echo
done
