:
# mv_keep_extension.sh
# --------------------
# -- Tim Riley
# --------------------

if [ "$#" -lt 2 ]
then
	(
	echo
	echo "Usage:	$0 file_from file_to [go]"
	echo
	echo "Example:	$0 organism_report species_report"
	echo "		This does the following:"
	echo "		mv organism_report.sh species_report.sh"
	echo "		mv organism_report.sql species_report.sql"
	echo "		mv organism_report.lst species_report.lst"
	echo
	) 1>&2
	exit 1
fi

for i in `ls $1\.*`
do
	from_file_extension=`extension.e $i leave`
	new_filename="$2.$from_file_extension"

	echo "mv $i\t$new_filename\c"
	if [ "$3" = "go" ]
	then
		mv $i $new_filename
		echo "\t\tdone"
	else
		echo "\t\tnot done"
	fi
done

if [ "$3" != "go" ]
then
	echo
	echo "Of OK type:	$0 $1 $2 go"
	echo
fi

exit 0
