:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make detail
fi

