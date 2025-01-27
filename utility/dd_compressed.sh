:
# dd_compressed.sh
# ----------------
# Tim Riley
# ----------------

if [ `whoami` != "root" ]
then
	echo "Only root can run this." 1>&2
	exit 1
fi

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 filesystem" 1>&2
	echo "Example: $0 /dev/hdb2" 1>&2
	exit 1
fi

today=`date.sh | column.e 0 | sed 's|/|-|g'`

filesystem=$1
output_device=/dev/nht0
sum_file=/tmp/dd_compressed_`basename $filesystem`_${today}.sum

mt -f $output_device rewind

dd if=$filesystem bs=10k 			|
gzip						|
tee $output_device				|
sum > $sum_file

mt -f $output_device rewind

echo "Sum file = $sum_file `cat $sum_file`"

exit 0
