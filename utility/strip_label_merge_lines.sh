:
# strip_label_merge_lines.sh
# --------------------------
#
# Tim Riley
# -----------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 label_delimiter number_lines_to_merge" 1>&2
	exit 1
fi

piece.e "$1" 1 | merge_n_lines.e n=$2

exit 0
