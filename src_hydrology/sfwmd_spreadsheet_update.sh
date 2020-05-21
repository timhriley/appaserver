#!/bin/bash
# ----------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/sfwmd_spreadsheet_update.sh
# ----------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 execute_yn" 1>&2
	exit 1
fi

execute_yn=$1

if [ "$execute_yn" = "y" ]
then
	sql=sql.e
	trim=cat
	html=cat
else
	sql=cat
	trim="queue_top_bottom_lines.e 50"
	html="html_paragraph_wrapper.e"
fi

key=station,datatype,measurement_date,measurement_time

(
while read input_line
do
	# Reconcile this with sfwmd_spreadsheet_parse.c, line 34.
	# -------------------------------------------------------
	station=`echo $input_line | piece.e '^' 0`
	datatype=`echo $input_line | piece.e '^' 1`
	measurement_date=`echo $input_line | piece.e '^' 2`
	measurement_time=`echo $input_line | piece.e '^' 3`
	last_validation_date=`echo $input_line | piece.e '^' 5`
	#provisional=`echo $input_line | piece.e '^' 6`
	db_key=`echo $input_line | piece.e '^' 7`

	# Update db_key
	# -------------
	if [ "$db_key" != "" ]
	then
		echo "$station^$datatype^$measurement_date^$measurement_time^last_validation_process^$db_key"
	fi

	# Update last_validation_date
	# ---------------------------
#	if [ "$last_validation_date" != "" -a "$provisional" != "provisional" ]
#	then
#		echo "$station^$datatype^$measurement_date^$measurement_time^last_validation_date^$last_validation_date"
#	fi

	if [ "$last_validation_date" != "" ]
	then
		echo "$station^$datatype^$measurement_date^$measurement_time^last_validation_date^$last_validation_date"
	fi

done
) |	update_statement.e table=measurement key=$key carrot=y	|
	$sql							|
	$trim							|
	$html							|
	cat

exit 0
