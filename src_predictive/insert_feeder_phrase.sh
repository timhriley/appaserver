#!/bin/bash
# -------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/insert_feeder_phrase.sh
# -------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "ERROR in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 4 ]
then
	echo "Usage: `basename.e $0 n` process account full_name feeder_phrase" 1>&2
	exit 1
fi

process="$1"
account="$2"
full_name="$3"
feeder_phrase="$4"

document_body.sh

echo "<h1>`echo $process | format_initial_capital.e`</h1>"

if [	"$account" = "" -o	\
	"$account" = "account" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please choose an account .</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

if [	"$full_name" = "" -o			\
	"$full_name" = "full_name" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please enter in the entity's full name.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

if [	"$feeder_phrase" = "" -o		\
	"$feeder_phrase" = "feeder_phrase" ]
then
	echo "<h2> `now.sh 19` </h2>"
	echo "<h3>Please enter in a feeder phrase.</h3>"
	echo "</body>"
	echo "</html>"
	exit 0
fi

street_address="any"

# Insert feeder_phrase
# --------------------
table="feeder_phrase"
field="feeder_phrase,nominal_account,full_name,street_address"

echo "$feeder_phrase^$account^$full_name^$street_address"	|
insert_statement.e table=$table field=$field del='^'		|
sql.e 2>&1							|
html_paragraph_wrapper.e

# Insert entity
# -------------
field="full_name,street_address"

echo "$full_name^$street_address"				|
insert_statement.e table=entity field=$field del='^'		|
sql.e 2>&1							|
grep -vi duplicate						|
html_paragraph_wrapper.e

echo "<h2> `now.sh 19` </h2>"
echo "<h3>Insert complete for: $feeder_phrase</h3>"

echo "</body>"
echo "</html>"

exit 0

