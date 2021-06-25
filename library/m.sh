:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make get_folder_data
fi

