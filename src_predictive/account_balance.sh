#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/account_balance.sh
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

echo "$0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 process_name as_of_date institution_full_name investment_purpose execute_yn" 1>&2
	exit 1
fi

process_name=$(echo "$1" | format_initial_capital.e)
input_as_of_date="$2"
institution_full_name="$3"
investment_purpose="$4"
execute_yn="$5"

get_as_of_date()
{
	input_as_of_date=$1

	if [ "$input_as_of_date" = "" -o "$input_as_of_date" = "as_of_date" ]
	then
		as_of_date=`date.e 0 | piece.e ':' 0`
	else
		as_of_date="$input_as_of_date"
	fi

	echo "$as_of_date"
}

get_institution_where()
{
	full_name=$1

	if [ "$full_name" = "" -o "$full_name" = "full_name" ]
	then
		where="1 = 1"
	else
		where="full_name = '$full_name'"
	fi

	echo "$where"
}

get_investment_purpose_where()
{
	investment_purpose=$1

	if [	"$investment_purpose" = "" -o		     \
		"$investment_purpose" = "investment_purpose" ]
	then
		where="1 = 1"
	else
		where="investment_purpose = '$investment_purpose'"
	fi

	echo "$where"
}

institution_where=$(get_institution_where $institution_full_name)
investment_purpose_where=$(get_investment_purpose_where $investment_purpose)
as_of_date=$(get_as_of_date $input_as_of_date)

html_title="$process_name as of $as_of_date"

heading="	institution_full_name,	\
		account_number,		\
		As Of Date,		\
		Book Value,		\
		Certificate Maturity,	\
		Investment Purpose"

justification="left,left,left,right,right,left"

account_process='echo "	select	full_name,				\
				account_number,				\
				certificate_maturity_date,		\
				investment_purpose			\
			from investment_account				\
			where	$institution_where and			\
				$investment_purpose_where		\
			order by	full_name,			\
					certificate_maturity_date;"    |\
			sql "^"'

function account_balance_record ()
{
	investment_account_record="$1"

	institution_full_name=`echo $investment_account_record | piece.e '^' 0`
	account_number=`echo $investment_account_record | piece.e '^' 1`
	certificate_maturity_date=`echo $investment_account_record | piece.e '^' 2`
	investment_purpose=`echo $investment_account_record | piece.e '^' 3`

	select="full_name, account_number, date, balance, '$certificate_maturity_date', '$investment_purpose'"
	from="account_balance"
	where="full_name = '$institution_full_name' and account_number = '$account_number' and date <= '$as_of_date 23:59:59'"
	order="date desc"

	echo "select $select from $from where $where order by $order;"	|
	sql '^'								|
	head -1
}

(
eval "$account_process"							|
while read investment_account_record
do
	account_balance_record "$investment_account_record"
done
)									|
piece_sum.e '^' 3							|
piece_commas_double.e 3 2						|
format_initial_capital.e						|
html_table.e "$html_title" "$heading" '^' "$justification" 		|
html_wrapper.e								|
cat

if [	"$input_as_of_date" != "$as_of_date" -a				\
	"$institution_where" = "1 = 1" -a				\
	"$investment_purpose_where" = "1 = 1"				]
then
	investment_sum=`						\
	(
	eval "$account_process"						|
	while read investment_account_record
	do
		account_balance_record "$investment_account_record"
	done
	)								|
	piece_sum.e '^' 3 y						|
	piece.e '^' 3`

	liability_sum=`							\
	(
	eval "$account_process"						|
	grep "\^taxable_retirement$"					|
	while read investment_account_record
	do
		account_balance_record "$investment_account_record"
	done
	)								|
	piece_sum.e '^' 3 y						|
	piece.e '^' 3`

	investment_transaction_driver	\
		$investment_sum		\
		$liability_sum		\
		$execute_yn

	if [ "$execute_yn" = "y" ]
	then
		echo "<h3>Process complete.</h3>"
	fi
fi

exit 0
