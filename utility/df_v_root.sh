:
# df_v_root.sh
# ------------

PATH=$PATH:/usr2/utility; export PATH

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 percent_free_to_report" 1>&2
	exit 1
fi

percent_free_to_report=$1

percent_free=`df -v | grep "^/ " | column.e 5 | piece.e '%' 0`

if [ "$percent_free" -le "$percent_free_to_report" ]
then
	echo `date` " $0 Warning: root percent free is $percent_free" > /dev/console
fi

exit 0
