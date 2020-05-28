:
# $APPASERVER_HOME/src_predictive/account_balance.sh
# --------------------------------------------------
# Freely available software. See appaserver.org
# --------------------------------------------------

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

content_type_cgi.sh

echo "$0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 ignored process_name as_of_date institution_full_name institution_street_address" 1>&2
	exit 1
fi

process_name=$(echo $2 | format_initial_capital.e)

as_of_date=$3
if [ "$as_of_date" = "" -o "$as_of_date" = "as_of_date" ]
then
	as_of_date=`date.e 0 | piece.e ':' 0`
fi

institution_full_name=$4
street_address=$5
if [ "$institution_full_name" = "" -o "$institution_full_name" = "full_name" ]
then
	institution_where="1 = 1"
else
	institution_where="full_name = '$institution_full_name' and street_address = '$street_address'"
fi

heading="institution_full_name, account_number, As Of Date, Book Value, Certificate Maturity"
justification="left, left, left, right, left"

account_process='echo "select full_name, street_address, account_number, certificate_maturity_date from investment_account where $institution_where order by full_name, street_address, certificate_maturity_date;" | sql "^"'

(
eval $account_process						|
while read record
do
	institution_full_name=`echo $record | piece.e '^' 0`
	street_address=`echo $record | piece.e '^' 1`
	account_number=`echo $record | piece.e '^' 2`
	certificate_maturity_date=`echo $record | piece.e '^' 3`

	select="full_name, account_number, date_time, book_value_balance, '$certificate_maturity_date'"
	from="equity_account_balance"
	where="full_name = '$institution_full_name' and street_address = '$street_address' and account_number = '$account_number' and date_time <= '$as_of_date 23:59:59'"
	order="date_time desc"

	echo "select $select from $from where $where order by $order;"	|
	sql '^'								|
	head -1

done
)									|
piece_sum.e '^' 3							|
piece_commas_in_double.e 3						|
format_initial_capital.e						|
html_table.e "$process_name" "$heading" '^' "$justification"		|
html_wrapper.e "/appaserver/template/style.css"				|
cat

exit 0
