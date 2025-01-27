:
# $APPASERVER_HOME/src_system/export_process_role.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

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

if [ $# -ne 2 ]
then
	echo "Usage: `basename.e $0 n` role output_sqlgz" 1>&2
	exit 1
fi

role=$1
output_sqlgz=$2

directory=`appaserver_data_directory`
date=`now.sh ymd`
input_export_process="${directory}/export_process_${application}_${date}.sh"

output_export_process="/tmp/export_process_$$.sql"

# Select ROLE_PROCESS.process
# ---------------------------
system_string="	select.sh role,process role_process '' '' |	\
		sed 's/\^/|/' |					\
		grep '^$role|' |				\
		piece.e '|' 1 |					\
		sort -u"

eval $system_string |
while read process_name
do
	export_process '' $process_name n >/dev/null
	grep '^insert' < $input_export_process >> $output_export_process
done

cat $output_export_process | gzip > $output_sqlgz
rm $output_export_process

exit 0
