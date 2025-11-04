:
if [ $# -ne 1 ]
then
	echo "Usage: `echo $0 | basename.e` URL" 1>&2
	exit 1
fi

wget	--mirror		\
	--convert-links		\
	--adjust-extension	\
	--page-requisites	\
	--no-parent $1

exit $?
