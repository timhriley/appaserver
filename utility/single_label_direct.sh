:
# utility/single_label_direct.sh
# ------------------------------

if [ "$#" -ne 8 ]
then
	echo "Usage: $0 first last street unit city state zip quantity" 1>&2
	exit 1
fi

first=$1
last=$2
street=$3
unit=$4
city=$5
state=$6
zip=$7
quantity=$8

exception=/dev/null

tmp_filename=/tmp/single_label_direct_$$.dat
> $tmp_filename

while [ $quantity -ne 0 ]
do
	echo "$first|$last|$street|$unit|$city|$state|$zip"		 |
	label.e up=1 tchars=26 startline=2 tlines=6 exception=$exception |
	cat >> $tmp_filename
	quantity=`expr $quantity - 1`
done

cat $tmp_filename | print_direct.sh
rm $tmp_filename
exit 0

