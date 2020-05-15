:
# $APPASERVER_HOME/src_predictive/feeder_upload_statistics.sh
# -----------------------------------------------------------
# Freely available software. See appaserver.org
# -----------------------------------------------------------

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

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 process_name output_medium" 1>&2
	exit 1
fi

process_name=$1

process_title=`echo "$process_name" | format_initial_capital.e`

output_medium=$2
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

# Constants
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"

heading="bank_upload_date_time,bank_upload_filename,min(bank_date),max(bank_date),count,sum(bank_amount)"
justification="left,left,right"

echo "$heading" > $output_file

statement="
select bank_upload_event.bank_upload_date_time, bank_upload_filename, min( bank_date ), max( bank_date ), count(1), sum( bank_amount ) from bank_upload, bank_upload_event where bank_upload.bank_upload_date_time = bank_upload_event.bank_upload_date_time group by bank_upload_event.bank_upload_date_time order by bank_upload_event.bank_upload_date_time desc;"

echo "$statement"			|
sql.e ','				|
cat >> $output_file

# Output
# ------
if [ "$output_medium" = "spreadsheet" -o "$output_medium" = "text_file" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
elif [ "$output_medium" = "table" ]
then
	cat $output_file				|
	grep -v "$heading"				|
	html_table.e "" "$heading" "," "$justification"
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit 0
