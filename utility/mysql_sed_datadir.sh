:
# $APPASERVER_HOME/utility/sed_data_directory.sh
# -------------------------------------------------------------
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 data_directory [index_directory]" 1>&2
	exit 1
fi

data_directory=$1

if [ "$#" -eq 2 ]
then
	index_directory=$2
fi

DD=$data_directory
ID=$index_directory

if [ "$data_directory" = "" -a "$index_directory" = "" ]
then
	cat
elif [ "$ID" != "" ]
then
	sed "s|ENGINE=MyISAM|data directory='$DD' index directory='$ID' &|"
else
	sed "s|ENGINE=MyISAM|data directory='$DD' &|"
fi

exit $?

