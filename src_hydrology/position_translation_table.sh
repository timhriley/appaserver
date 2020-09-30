:
# position_translation_table.sh
# ----------------------------------------------------------------
# Tim Riley
# ----------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 entity station" 1>&2
	exit 1
fi

entity=$1
station=$2

table_name=`get_cr10_parse_folder_name.sh $entity`

echo "	select record_type, datatype, position		\
	from $table_name				\
	where station = '$station'			\
	order by record_type, position;"		|
sql '|'							|
# ------------------------------------------		|
# Subtract 4 to provide absolute positioning		|
# ------------------------------------------		|
sed 's/$/|4/'						|
piece_arithmetic.e 2 3 subtraction '|'			|
sed 's/\.00*$//g'					|
piece_inverse.e 2,3 '|'					|
merge_position_translation_table $station		|
cat

exit 0
