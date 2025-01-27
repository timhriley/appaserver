:
# next_filename.sh
# ----------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 filename_prefix" 1>&2
	exit 1
fi

latest=`ls -t ${1}_* | head -1 | sed 's/.*_//'`

if [ "$latest" = "" ]
then
	echo "${1}_1"
	echo "using ${1}_1" 1>&2
else
	echo "${1}_`expr $latest + 1`"
fi
