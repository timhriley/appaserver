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

output="cat"
#output="sql"

# Returns 0 if ELEMENT.element exists
# -----------------------------------
table_column_exists.sh element element

if [ $? -ne 0 ]
then
	exit 0
fi

integrity()
{
	table="$1"
	column1="$2"
	column2="$3"

	count=$(select.sh "count( $column1 )" $table)
	distinct_count=$(select.sh "count( distinct $column1 )" $table)

	if [ $count -ne ${distinct_count} ]
	then
		echo "ERROR in `basename.e $0 n`: duplicate ${column1}s in $table" 1>&2
		echo "You need to execute the Merge Purge process on $table." 1>&2
		echo "To find the duplicates, execute:" 1>&2
		echo "select.sh $column1,$column2 $table | sort | less" 1>&2
		exit 1
	fi
}

drop_index()
{
	table=$1
	index_name=$2

	echo "alter table $table drop index ${index_name};" | $output
}

update_table_column()
{
	table=$1
	column=$2

	echo "update table_column set primary_key_index = null, display_order = 1 where table_name = '$table' and column_name = '$column';" | $output
}

drop_column()
{
	table=$1
	column=$2

	echo "alter table $table drop column $column;" | $output
}

delete_table_column()
{
	table=$1
	column=$2

	echo "delete from table_column where table_name = '$table' and column_name = '$column';" | $output
}

get_index_name()
{
	table=$1

	index_name=$(echo "show index from $table;"	|
		     sql				|
		     piece.e '^' 2			|
		     grep -v additional_unique		|
		     sort -u)

	echo $index_name
}

create_index()
{
	table=$1

	statement=$(create_index.sh $table | grep -v additional)

	echo $statement | $output
}

# Table=entity
# ------------
table=entity
integrity $table full_name street_address
update_table_column $table street_address
index_name=$(get_index_name $table)
drop_index $table $index_name
create_index $table

for table in	account_balance		\
		appaserver_user		\
		feeder_account		\
		feeder_load_event	\
		feeder_phrase		\
		feeder_row		\
		financial_institution	\
		investment_account	\
		journal			\
		login_default_role	\
		prior_fixed_asset	\
		role_appaserver_user	\
		self			\
		session			\
		transaction		\
		vendor
do
	delete_table_column $table street_address
	drop_column $table street_address
	index_name=$(get_index_name $table)
	drop_index $table $index_name
	create_index $table
done

exit 0
