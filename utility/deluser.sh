:
# $APPASERVER_HOME/utility/deluser.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 username" 1>&2
	exit 1
fi

username=$1

userdel.sh $username

exit 0
