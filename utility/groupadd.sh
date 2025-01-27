:
# $APPASERVER_HOME/utility/groupadd.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 group user [user...]" 1>&2
	exit 1
fi

group=$1

shift

while [ $# -ge 1 ]
do
	username=$1
	sudo usermod -a -G $group $username
	shift
done
 
exit 0

