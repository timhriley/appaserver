:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make statistics_folder
fi

