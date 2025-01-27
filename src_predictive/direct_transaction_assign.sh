#!/bin/bash
# ------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/direct_transaction_assign.sh
# ------------------------------------------------------------

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

if [ "$#" -lt 6 ]
then
	echo "Usage: `basename.e $0 n` process bank_date bank_description full_name street_address transaction_date_time [fund]" 1>&2
	exit 1
fi

echo "$0 $*" 1>&2

process=$1
bank_date="$2"
bank_description="$3"
full_name="$4"
street_address="$5"
transaction_date_time="$6"

if [ "$#" -eq 6 ]
then
	fund=$6
else
	fund=""
fi

content_type_cgi.sh

cat << all_done
<html>
<head>
<link rel=stylesheet type="text/css" href="/appaserver/$application/style.css">
</head>
<body>
all_done
 
echo "<h1>`echo $process | format_initial_capital.e`</h1>"

bank_upload_transaction_insert "${bank_date}^${bank_description}^${full_name}^${street_address}^${transaction_date_time}" "$fund"		  |
html_paragraph_wrapper.e					  |
cat

bank_upload_sequence_propagate.sh ${bank_date} | sql.e
bank_upload_balance_propagate.sh ${bank_date} | sql.e

echo "<h3>Process complete</h3>"
echo "</body>"
echo "</html>"

exit 0

