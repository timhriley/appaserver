:
# $APPASERVER_HOME/utility/iotop.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

flags="-o -b -n1 -k"

if [ "$#" -ne 3 ]
then
	echo "Usage: `echo $0 | basename.e` seconds iterations withclear_yn" 1>&2
	exit 1
fi

seconds=$1
iterations=$2
withclear_yn=$3

binary_count.e $iterations					|
while read ignore
do
	if [ "$withclear_yn" = "y" ]
	then
		sudo sync
		# -------------------------------------
		# Clear pagecache, dentries and inodes.
		# -------------------------------------
		sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
		# -----------------
		# Clear swap space.
		# -----------------
		sudo sh -c 'swapoff -a && swapon -a'
		sudo iotop $flags | trim.e 80

		sleep $seconds
		echo
	else
		sudo iotop $flags | trim.e 80
		sleep $seconds
		echo
	fi
done

exit 0
