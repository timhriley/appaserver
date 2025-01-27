:
# untarbz.sh
# ----------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 filename [filename...]" 1>&2
	exit 1
fi

while [ "$#" -gt 0 ]
do
	filename=$1
	echo "Extracting: $filename"
	bunzip2 -c $filename | tar xvf -

	shift

done


exit 0
