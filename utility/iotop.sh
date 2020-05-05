:
# $APPASERVER_HOME/utility/iotop.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -eq 0 ]
then
	withclear_yn=n
elif [ "$#" -eq 1 ]
then
	if [ "$1" != "y" -a "$1" != "n" ]
	then
		echo "Usage: `echo $0 | basename.e` [withclear_yn]" 1>&2
		echo "Note:  withclear_yn defalts to n" 1>&2
		exit 1
	fi
	withclear_yn=$1
fi

while [ true ]
do
	if [ withclean_yn = "y" ]
	then
		sudo sync
		# -------------------------------------
		# Clear pagecache, dentries and inodes.
		# -------------------------------------
		sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
		sudo iotop -o -b -n1 | trim.e 80
		sleep 5
		echo
	else
		sudo iotop -o -b -n1 | trim.e 80
		sleep 5
		echo
	fi
done
