:
# -----------------------------------------
# $APPASERVER_HOME/utility/ufw_auth_deny.sh
# -----------------------------------------

grep 'Failed password for invalid user' /var/log/auth.log	|
column.e 12							|
grep -v port							|
sort -u								|
while read ip
do
	echo $ip
done

exit 0
