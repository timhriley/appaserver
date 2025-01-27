:
# ftp_put.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -lt 4 ]
then
	echo "Usage: $0 host directory 'filespec' username [password]" 1>&2
	exit 1
fi

host=$1
directory=$2
filespec=$3
username=$4

if [ "$#" -eq 5 ]
then
	password=$5
else
	stty -echo
	echo -n "Password: "
	read password
	echo ""
	stty echo
fi

(
	echo "open $host"
	echo "user $username $password"
	echo "prompt"
	echo "binary"
	echo "cd $directory"
	echo "mput \""$filespec"\""
	echo "close"
) | ftp -n -v

exit 0
