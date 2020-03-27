:
# -----------------------------------------
# $APPASERVER_HOME/utility/ufw_auth_deny.sh
# -----------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 execute_yn" 1>&2
	exit 1
fi

execute_yn=$1

grep 'Failed password for invalid user' /var/log/auth.log	|
while read input
do
	echo $input
done

exit 0
