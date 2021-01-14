#!/bin/bash

echo $0 $* 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 product_name" 1>&2
	exit 1
fi

product_name="$1"

product_name_escaped=`echo "$product_name" | escape_character.e "'"`

echo "	update product							\
	set refund_total =						\
		( select sum( refund_amount )				\
			from product_refund				\
			where product_refund.product_name =		\
				product.product_name )			\
	where	product_name = '$product_name_escaped';"		|
sql

exit 0

