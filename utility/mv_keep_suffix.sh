#!/bin/ksh
# mv_keep_suffix.sh
# --------------------
# -- Tim Riley
# --------------------

if [ "$#" -lt 2 ]
then
	(
	echo
	echo "Usage:	$0 prefix_from prefix_to [go]"
	echo
	echo "Example:	$0 organism_ species_"
	echo "		This does the following:"
	echo "		mv organism_report.sh species_report.sh"
	echo "		mv organism_report.sql species_report.sql"
	echo "		mv organism_report.lst species_report.lst"
	echo
	) 1>&2
	exit 1
fi

prefix_from=$1
prefix_to=$2

for i in `ls ${prefix_from}*`
do
	suffix=`suffix_extract.e $i ${prefix_from}`
	new_filename="${prefix_to}${suffix}"

	echo "mv $i\t$new_filename\c"
	if [ "$3" = "go" ]
	then
		mv $i $new_filename
		echo "\tdone"
	else
		echo "\tnot done"
	fi
done

if [ "$3" != "go" ]
then
	echo
	echo "Of OK type: " `basename $0`" $1 $2 go"
	echo
fi

exit 0
