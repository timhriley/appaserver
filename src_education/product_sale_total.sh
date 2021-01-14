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
	set sale_total =						\
		( select sum( extended_price )				\
			from product_sale				\
			where product_sale.product_name =		\
				product.product_name )			\
	where	product_name = '$product_name_escaped';"		|
sql

exit 0

