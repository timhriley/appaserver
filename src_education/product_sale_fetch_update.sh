#!/bin/bash

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 product_name [set_yn]" 1>&2
	exit 1
fi

product_name="$1"

if [ "$#" -eq 2 -a "$2" = 'y' ]
then
	do_set=1
else
	do_set=0
fi

product_name_escaped=`echo "$product_name" | escape_character.e "'"`

function product_set()
{
	product_name="$1"
	sale_total=$2

	table="product"
	key="product_name"

	data=`echo "$product_name^sale_total^$sale_total"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_sale_total()
{
	product_name_escaped="$1"

	sale_total=`							\
		echo "	select sum( extended_price )			\
			from product_sale				\
			where product_name = '$product_name_escaped';"	|
	sql`
}

fetch_sale_total "$product_name_escaped"

if [ "$do_set" -eq 1 ]
then
	product_set "$product_name" $sale_total
else
	echo "$sale_total"
fi

exit 0

