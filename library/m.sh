:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_choose_folder_process_menu
fi

