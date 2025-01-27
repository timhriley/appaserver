:
# $APPASERVER_HOME/src_predictive/export_process_predict.sh
# -----------------------------------------------------------------
# Exports the feeder_load processes that aren't assigned to a role.
# -----------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# -----------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ $# -ne 1 ]
then
	echo "Usage: `basename.e $0 n` output_sql" 1>&2
	exit 1
fi

output_sql=$1

directory=`appaserver_data_directory`
date=`now.sh ymd`
input_export_process="${directory}/export_process_${application}_${date}.sh"

rm $output_sql 2>/dev/null

where="command_line like 'feeder_load%'"

# Select PROCESS.process
# ----------------------
system_string="select.sh process process \"$where\""

eval $system_string |
while read process_name
do
	export_process '' $process_name n >/dev/null
	grep '^insert' < $input_export_process >> $output_sql
done

exit 0
