:
# $APPASERVER_HOME/utility/sed_data_directory.sh
# ----------------------------------------------
# Freely available software: see Appaserver.org
# ----------------------------------------------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 data_directory [index_directory]" 1>&2
	exit 1
fi

data_directory=$1

if [ "$#" -eq 2 ]
then
	index_directory=$2
else
	index_directory=$1
fi

DD=$data_directory
ID=$index_directory

sed "s|ENGINE=MyISAM|data directory='$DD',index directory='$ID' &|"

exit $?

