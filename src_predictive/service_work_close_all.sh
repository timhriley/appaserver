#!/bin/bash
# ---------------------------------------------------------
# $APPASERVER_HOME/src_predictive/service_work_close_all.sh
# ---------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 ignored login_name" 1>&2
	exit 1
fi

login_name=$2

now=`now.sh colon | piece_inverse.e 2 ':'`

function hourly_service_work_close()
{
	application=$1
	login_name=$2
	now=$3

	table="hourly_service_work"

	key="		full_name,
			street_address,
			sale_date_time,
			service_name,
			description,
			begin_work_date_time"

	select="	full_name,
			street_address,
			sale_date_time,
			service_name,
			description,
			substr( begin_work_date_time, 1, 16 )"

	where="login_name = '${login_name}' and end_work_date_time is null"

	echo "select ${select} from $table where $where;"		|
	sql.e								|
	while read record
	do
		echo "${record}^end_work_date_time^${now}"		|
		update_statement.e table=$table key="$key" carrot=y	|
		sql.e

		full_name=`echo $record | piece.e '^' 0`
		street_address=`echo $record | piece.e '^' 1`
		sale_date_time=`echo $record | piece.e '^' 2`
		service_name=`echo $record | piece.e '^' 3`
		description=`echo $record | piece.e '^' 4`
		begin_work_date_time=`echo $record | piece.e '^' 5`

		post_change_hourly_service_work		\
			"${application}"		\
			"${full_name}"			\
			"${street_address}"		\
			"${sale_date_time}"		\
			"${service_name}"		\
			"${description}"		\
			"${begin_work_date_time}"	\
			update				\
			preupdate_end_work_date_time
	done
}

function fixed_service_work_close()
{
	application=$1
	login_name=$2
	now=$3

	table="fixed_service_work"

	key="		full_name,
			street_address,
			sale_date_time,
			service_name,
			begin_work_date_time"

	select="	full_name,
			street_address,
			sale_date_time,
			service_name,
			substr( begin_work_date_time, 1, 16 )"

	where="login_name = '${login_name}' and end_work_date_time is null"

	echo "select ${select} from $table where $where;"		|
	sql.e								|
	while read record
	do
		echo "${record}^end_work_date_time^${now}"		|
		update_statement.e table=$table key="$key" carrot=y	|
		sql.e

		full_name=`echo $record | piece.e '^' 0`
		street_address=`echo $record | piece.e '^' 1`
		sale_date_time=`echo $record | piece.e '^' 2`
		service_name=`echo $record | piece.e '^' 3`
		begin_work_date_time=`echo $record | piece.e '^' 4`

		post_change_fixed_service_work		\
			"${application}"		\
			"${full_name}"			\
			"${street_address}"		\
			"${sale_date_time}"		\
			"${service_name}"		\
			"${begin_work_date_time}"	\
			update				\
			preupdate_end_work_date_time
	done
}

hourly_service_work_close "$application" "$login_name" "$now"
fixed_service_work_close "$application" "$login_name" "$now"

exit 0
