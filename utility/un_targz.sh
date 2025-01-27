:
# un_targz.sh
# -----------
#
# Input: 	filename.tar.gz
# Process:	This program un-zips a file then un-tars it.
# Output:	The files extracted.
#
# Tim Riley
# ----------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 filename.tar.gz" 1>&2
	echo "Note:  Filenames may be filename.tgz" 1>&2
	exit 1
fi

cat $1 | gzip -d | tar -xvf -

exit $?
