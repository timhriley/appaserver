:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 command" 1>&2
	exit 1
fi

command=$1

filename=`find /usr/share/man/ -name "${command}\.[1-9]*" | head`
zcat $filename | nroff -man
