#!/bin/bash
# ---------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_account.sh
# ---------------------------------------------------------

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

echo $0 $* 1>&2

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 many_table subclassification [element]" 1>&2
	exit 1
fi

many_table=$1
subclassification="$2"

if [ "$#" -eq 3 -a "$3" != "" -a "$3" != "element" ]
then
	element="$3"
else
	element=""
fi

# Select
# ======

# Select chart_account_number?
# ----------------------------
select_chart_account_number=0

table_column_exists.sh account chart_account_number

# Zero means true
# ---------------
exists_chart_account_number=$?

if [ $exists_chart_account_number = 0 ]
then
	chart_account_number_count=`echo			\
		"select count(1)				\
		 from account					\
		 where chart_account_number is not null;"	|
		 sql.e`

	if [ "$chart_account_number_count" -ge 1 ]
	then
		# One means false
		# ---------------
		select_chart_account_number=1
	fi
fi

if [ $select_chart_account_number -eq 1 ]
then
	select="concat( account, '|', account, '---', ifnull( chart_account_number, '' ) )"
else
	select="concat( account, '|', account, ' [', account.subclassification, ']' )"
fi

# From
# ----
from="account"

# Order
# -----
order="account"

# Where
# -----
if [ "$subclassification" = "" -o "$subclassification" = "subclassification" ]
then
	subclassification_where="1 = 1"
else
	subclassification_where="account.subclassification = '$subclassification'"
fi

if [ "$element" != "" ]
then
	subclassification_join="account.subclassification = subclassification.subclassification"

	element_portion="subclassification.element = 'expense'"

	element_where="${subclassification_join} and ${element_portion}"

	# Override from clause
	# --------------------
	from="account,subclassification"
else
	element_where="1 = 1"
fi

one2m_where="1 = 1"

if [ "$many_table" = "self" ]
then
	one2m_where="account.subclassification = 'cash'"
fi

if [ "$many_table" = "inventory" ]
then
	one2m_where="account.subclassification = 'inventory' or account.subclassification = 'cost_of_goods_sold'"
fi

if [ "$many_table" = "investment_account" ]
then
	one2m_where="account.subclassification like '%investment%'"
fi

if [	"$many_table" = "fixed_asset" -o		\
	"$many_table" = "prior_fixed_asset" ]
then
	one2m_where="account.subclassification = 'property_plant_equipment' or account.subclassification = 'fixed_asset'"
fi

if [	"$many_table" = "fixed_service" -o	\
	"$many_table" = "hourly_service" 	]
then
	one2m_where="account.subclassification like '%revenue%'"
fi

if [ "$many_table" = "liability_account_entity" ]
then
	one2m_where="account.subclassification = 'current_liability'"
fi

if [ "$many_table" = "supply" ]
then
	one2m_where="account.subclassification = 'supply_expense'"
fi

if [ "$many_table" = "service_purchase" ]
then
	one2m_where="account.subclassification = 'service_expense'"
fi

if [ "$many_table" = "offering" ]
then
	one2m_where="account.subclassification like '%revenue%'"
fi

where="$one2m_where and $subclassification_where and $element_where"

echo "select $select from $from where $where order by $order;"		|
sql.e									|
string_initial_capital.e '|' 1						|
cat

exit 0
