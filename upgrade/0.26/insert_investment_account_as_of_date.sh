#!/bin/bash
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
	echo "$ . set_database" 1>&2
	exit 1
fi

# Returns 0 if INVESTMENT_ACCOUNT.full_name exists
# ------------------------------------------------
table_column_exists.sh investment_account full_name

if [ $? -ne 0 ]
then
	exit 0
fi

(
echo "alter table investment_account add as_of_date date;" | tee_appaserver.sh | sql.e 2>&1
echo "insert into appaserver_column (column_name,column_datatype,width,float_decimal_places,hint_message) values ('as_of_date','date',10,null,null);" | sql.e
echo "insert into table_column (table_name,column_name,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,additional_unique_index_yn,additional_index_yn,insert_required_yn,lookup_required_yn,default_value) values ('investment_account','as_of_date',null,4,'y',null,'y',null,null,null,null,null);" | sql.e
echo "update table_column set display_order = 3 where table_name = 'investment_account' and column_name = 'balance_latest';" | sql.e
echo "update table_column set display_order = 5 where table_name = 'investment_account' and column_name = 'certificate_maturity_months';" | sql.e
echo "update table_column set display_order = 6 where table_name = 'investment_account' and column_name = 'certificate_maturity_date';" | sql.e
echo "update table_column set display_order = 7 where table_name = 'investment_account' and column_name = 'interest_rate';" | sql.e
) 2>&1 | grep -vi duplicate

select.sh "full_name,account_number" investment_account |
while read row
do
	full_name=`echo "$row" | piece.e '^' 0`
	account_number=`echo "$row" | piece.e '^' 1`

	where="full_name='$full_name' and account_number='$account_number'"
	as_of_date=`select.sh 'max(date)' account_balance "$where"`

	$APPASERVER_HOME/src_predictive/post_change_account_balance	\
		fund_name						\
		"$full_name"						\
		contact_key						\
		"$account_number"					\
		"$as_of_date"						\
		update
done

exit 0

