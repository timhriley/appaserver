:
# $APPASERVER_HOME/src_predictive/select_tax_form_line.sh
# -------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_database" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 ignored tax_form" 1>&2
	exit 1
fi

tax_form=$2

key=".tax_form_line "

select="distinct concat( '$key', tax_form_line ), tax_form_description, itemize_accounts_yn"

from="tax_form_line"

where="tax_form = '$tax_form'"

#order="tax_form_line"

echo "select $select from $from where $where;"			|
sql.e 								|
sort -n -k2							|
while read record
do
	echo $record
	tax_form_line=`echo "$record" | sed "s/$key//" | piece.e '^' 0`

	select_tax_form_line_account.sh	$application		\
					"$tax_form"		\
					"$tax_form_line"
done

exit 0

