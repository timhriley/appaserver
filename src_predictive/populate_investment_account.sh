:
# $APPASERVER_HOME/src_predictive/populate_investment_account.sh
# --------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
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

echo $0 $* 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 where" 1>&2
	exit 1
fi

parameter_where="$1"

table="investment_account"

select="concat( full_name, '^', street_address, '^', account_number, ' [', investment_classification, ']' )"

if [ "$parameter_where" = "where" -o "$parameter_where" = "" ]
then
	where="1 = 1"
else
	where=$parameter_where
fi

echo "select $select from $table where $where;" | sql.e

exit 0
