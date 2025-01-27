:
# $APPASERVER_HOME/utility/Rscript.sh
# --------------------------------------------------------------
# No warranty and freely available software. See: appaserver.org
# --------------------------------------------------------------
if [ "$#" -lt 1 ]
then
	echo "Usage: $0 scriptfile.R [stderr_file]" 1>&2
	exit 1
fi

scriptfile=$1

if [ "$#" -eq 2 ]
then
	stderr_file=$2
fi

if [ "$stderr_file" != "" ]
then
	Rscript $scriptfile 2>$stderr_file
else
	Rscript $scriptfile
fi

exit 0
