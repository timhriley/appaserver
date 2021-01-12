#!/bin/bash

echo $0 $* 1>&2

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
	refund_total=$2

	table="product"
	key="product_name"

	data=`echo "$product_name^refund_total^$refund_total"`

	echo $data 						|
	update_statement table=$table key=$key carrot=y 	|
	sql
}

function fetch_refund_total()
{
	product_name_escaped="$1"

	refund_total=`							\
		echo "	select sum( refund_amount )			\
			from product_refund				\
			where product_name = '$product_name_escaped';"	|
	sql`
}

fetch_refund_total "$product_name_escaped"

if [ "$do_set" -eq 1 ]
then
	product_set "$product_name" $refund_total
else
	echo "$refund_total"
fi

exit 0

