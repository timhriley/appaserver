:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 hydrology_file execute_yn" 1>&2
	exit 1
fi

hydrology_file=$1
execute_yn=$2

if [ ! -d untar ]
then
	mkdir untar
else
	rm untar/* 2>/dev/null
fi

if [ "$execute_yn" = "y" ]
then
	cd untar
	tar xzf ../$hydrology_file
else
	echo "\
	cd untar\n\
	tar xzf ../$hydrology_file"
fi

exit $?
