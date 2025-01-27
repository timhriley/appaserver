:
# ------------------
# utility/emailin.sh
# ------------------
current_ip_address=`current_ip_address.sh 2>/dev/null`
echo "Current IP Address: $current_ip_address"
with_deny_process="cat"

if [ "$#" -eq 1 ]
then
	if [ "$1" = "withdeny" ]
	then
		with_deny_process="xargs.e sudo ufw insert 1 deny to any port 25 from {}"
	else
		echo "Usage $0 [withdeny]" 1>&2
		exit 1
	fi
fi

grep 'connect from' /var/log/mail.log				|
column.e 7							|
piece.e '[' 1							|
piece.e ']' 0							|
sort -u								|
grep -v "$current_ip_address"					|
breakin_whitelist.e 2>/dev/null					|
$with_deny_process						|
cat

if [ "$with_deny_process" != "cat" ]
then
	sudo -s 'echo > /var/log/mail.log'
fi

exit 0
