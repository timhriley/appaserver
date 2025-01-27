#!/bin/bash
# ----------------------------------------------------------
# $APPASERVER_HOME/src_predictive/activity_work_close_all.sh
# ----------------------------------------------------------

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

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application login_name begin_work_date_time" 1>&2
	exit 1
fi

login_name=$2
begin_work_date_time=$3

now=`now.sh colon | piece_inverse.e 2 ':'`

function activity_work_close()
{
	application=$1
	login_name=$2
	begin_work_date_time=$3
	now=$4

	table="activity_work"

	key="		login_name,
			begin_work_date_time"

	select="	login_name,
			substr( begin_work_date_time, 1, 16 )"

	where="		login_name = '${login_name}' and
			begin_work_date_time <> '$begin_work_date_time' and
			end_work_date_time is null"

	echo "select ${select} from $table where $where;"		|
	sql.e								|
	while read record
	do
		echo "${record}^end_work_date_time^${now}"		|
		update_statement.e table=$table key="$key" carrot=y	|
		sql.e

		login_name=`echo $record | piece.e '^' 0`
		begin_work_date_time=`echo $record | piece.e '^' 1`

		post_change_activity_work		\
			"${application}"		\
			"${login_name}"			\
			"${begin_work_date_time}"	\
			update
	done
}

activity_work_close "$application" "$login_name" "$begin_work_date_time" "$now"

exit 0
