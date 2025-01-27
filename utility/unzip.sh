:
# sudo apt-get install p7zip-full

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 file.zip" 1>&2
	exit 1
fi

7z e $1

exit $?
