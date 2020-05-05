:
# $APPASERVER_HOME/utility/iotop.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

while [ true ]
do
	sudo sync
	# Clear pagecache, dentries and inodes.
	# -------------------------------------
	sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
	sudo iotop -o -b -n1 | trim.e 80
	sleep 5
	echo
done
