:

if [ $# -ne 1 ]
then
	echo "Usage: $0 subdirectory" 1>&2
	exit 1
fi

subdirectory=$1
document_root=`grep document_root /etc/appaserver.config | piece.e '=' 1`

cd $document_root/$subdirectory
ls -1

exit 0
