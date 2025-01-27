:
# $APPASERVER_HOME/utility/adduser.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 username sudo_yn [additional_group]" 1>&2
	exit 1
fi

username=$1
sudo_yn=$2

if [ "$#" -eq 3 ]
then
	additional_group=$3
fi

useradd.sh $username "$sudo_yn" $additional_group

exit $?

