:
# remove_duplicate_rows.sh
#
# Tim Riley
# --------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 table primarykeys schema" 1>&2
	exit 1
fi

remove_duplicate_rows.e $1 $2 | sqlplus `get_uid_pwd.e $3`
