:
# $APPASERVER_HOME/src_appaserver/select.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

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

if [ "$#" -lt 1 ]
then
	echo "Usage: `basename.e $0 n` \*|attribute[,attribute] [table] [where] [order]" 1>&2
	exit 1
fi

select="$1"

if [ "$#" -gt 1 ]
then
	table="$2"
else
	table=""
fi

if [ "$#" -gt 2 ]
then
	where="$3"

	if [ "$where" = "" -o "$where" = "where" ]
	then
		where=""
	fi
else
	where=""
fi

if [ "$where" != "" ]
then
	where=`echo $where | sed 's/^where //'`
fi

if [ "$#" -gt 3 ]
then
	order="$4"

	if [ "$order" = "" -o "$order" = "order" -o "$order" = "none" ]
	then
		order=""
	fi
else
	order=""
fi

if [ "$select" = "*" -o "$select" = "select" ]
then
	select=`attribute_list $table | joinlines.e ','`
fi

if [ "$order" = "select" ]
then
	order="$select"
fi

if [ "$table" = "" ]
then
	echo "select $select;" | sql
elif [ "$where" = "" ]
then
	echo "select $select from $table;" | sql
elif [ "$order" = "" ]
then
	echo "select $select from $table where $where;" | sql
else
	echo "select $select from $table where $where order by $order;" | sql
fi

exit 0

