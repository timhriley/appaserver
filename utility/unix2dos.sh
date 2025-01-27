:
# utility/unix2dos.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 unix_filename" 1>&2
	exit 1
fi

unix_file=$1

if [ ! -f $unix_file ]
then
	echo "$0: $unix_file doesn't exist." 1>&2
	exit 1
fi

temp_file=/tmp/unix2dos_$$.txt

cat $unix_file | unix2dos > $temp_file

if [ "$?" -ne 0 ]
then
	echo "$0 exiting early." 1>&2
	exit 1
fi

mv -f $temp_file $unix_file

exit 0
