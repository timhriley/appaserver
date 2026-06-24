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

if [ "$application" != "appahost" ]
then
	exit 0
fi

#output="cat"
output="sql"

integrity()
{
	table="$1"
	column1="$2"
	column2="$3"

	count=$(select.sh "count( $column1 )" $table)
	distinct_count=$(select.sh "count( distinct $column1 )" $table)

	if [ $count -ne ${distinct_count} ]
	then
		echo "ERROR in `basename.e $0 n`: duplicate ${column1}s in ${table}." 1>&2
		echo "You need to execute the Merge Purge process on $table." 1>&2
		echo "To find the duplicates, execute:" 1>&2
		echo "\$ select.sh $column1,$column2 $table | sort | less" 1>&2
		exit 1
	fi
}

update_street_address()
{
	table=$1
	source_column=$2
	destination_column=$3

	echo "update $table set $destination_column = $source_column;" | $output
}

drop_index()
{
	table=$1
	index_name=$2

	echo "alter table $table drop index ${index_name};" | $output
}

rename_column()
{
	table=$1
	old_column=$2
	new_column=$3

	echo "alter table $table change $old_column $new_column char(60);" | $output
}

update_table_column_display_order()
{
	table=$1
	column=$2

	echo "update table_column set primary_key_index = null, display_order = 1 where table_name = '$table' and column_name = '$column';" | $output
}

insert_column()
{
	column=$1
	datatype=$2
	width=$3

	echo "insert into appaserver_column (column_name,column_datatype,width) values ( '$column','$datatype',$width );" | $output
}

add_column()
{
	table=$1
	column=$2
	datatype=$3

	echo "alter table $table add column $column $datatype;" | $output
}

insert_table_column()
{
	table=$1
	column=$2
	primary_key_index=$3
	default_value=$4

	echo "insert into table_column ( table_name,column_name,primary_key_index,default_value ) values ( '$table','$column',$primary_key_index,'$default_value' );" | $output
}

update_table_column()
{
	table=$1
	old_column=$2
	new_column=$3

	echo "update table_column set column_name = '$new_column' where table_name = '$table' and column_name = '$old_column';" | $output
}

update_foreign_column()
{
	old_foreign_column=$1
	new_foreign_column=$2

	echo "update foreign_column set foreign_column = '$new_foreign_column' where foreign_column = '$old_foreign_column';" | $output
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
update_table_column_display_order $table street_address
insert_column contact_key character 60
add_column $table contact_key 'char(60)'
update_street_address $table street_address contact_key
insert_table_column entity contact_key 2 unknown
index_name=$(get_index_name $table)
drop_index $table $index_name
create_index $table

# For each of the remaining tables
# --------------------------------
for table in	accrual					\
		activity_work				\
		appaserver_user				\
		customer				\
		feeder_account				\
		feeder_load_event			\
		feeder_phrase				\
		feeder_row				\
		financial_institution			\
		fixed_asset_inventory			\
		fixed_asset_inventory_depreciation	\
		fixed_service_sale			\
		fixed_service_work			\
		hourly_service_sale			\
		hourly_service_work			\
		journal					\
		liability_account_entity		\
		login_default_role			\
		role_appaserver_user			\
		sale					\
		self					\
		session					\
		transaction				\
		vendor
do
	update_table_column $table street_address contact_key
	rename_column $table street_address contact_key
done

update_foreign_column street_address contact_key
update_foreign_column appaserver_street_address appaserver_contact_key

exit 0
