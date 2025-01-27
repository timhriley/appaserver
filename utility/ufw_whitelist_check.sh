:
# -----------------------------------------------
# $APPASERVER_HOME/utility/ufw_whitelist_check.sh
# -----------------------------------------------

whitelist=/tmp/ufw_whitelist.dat

whitelist_check_b()
{
	ufw_status_class_b=/tmp/ufw_status_class_b.dat
	sudo ufw status					|
	grep DENY					|
	grep '/16$'					|
	column.e 2					|
	piece.e '.' 0,1 > $ufw_status_class_b
	
	for i in `cat $ufw_status_class_b`
	do
		grep "^${i}" $whitelist
	done
}

whitelist_check_c()
{
	ufw_status_class_c=/tmp/ufw_status_class_c.dat
	sudo ufw status					|
	grep DENY					|
	grep '/24$'					|
	column.e 2					|
	piece.e '.' 0,1 > $ufw_status_class_c
	
	for i in `cat $ufw_status_class_c`
	do
		grep "^${i}" $whitelist
	done
}

ufw_whitelist.e > $whitelist
whitelist_check_b
whitelist_check_c

exit 0
