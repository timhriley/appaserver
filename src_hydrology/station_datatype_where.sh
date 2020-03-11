#/bin/bash
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 station_list datatype_list" 1>&2
	echo "Note: lists are comma separated." 2>&1
	exit 1
fi

station_list=$1
datatype_list=$2

i=0

echo "("

while [ true ]
do
	station=`echo $station_list | piece.e , $i 2>/dev/null`

	if [ "$station" = "" ]
	then
		break
	fi

	datatype=`echo $datatype_list | piece.e , $i`

	if [ $i -ge 1 ]
	then
		echo "or"
	fi

	or_sequence.e "station,datatype" "$station,$datatype"

	i=`expr $i + 1`

done

echo ")"

exit 0
