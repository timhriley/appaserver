#!/bin/bash
# ---------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_tax_form_line.sh
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

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 ignored [where]" 1>&2
	exit 1
fi

echo $0 $* 1>&2

if [ $# -eq 2 -a "$2" != "where" -a "$2" != "" ]
then
	where=$2
else
	where="1 = 1"
fi

table=tax_form_line
select="tax_form,concat( tax_form_line, ' [', tax_form_description, ']' )"

echo "select $select from $table where $where order by tax_form_line;"	|
sql.e

exit 0
