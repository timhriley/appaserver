:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_delete_folder_block
fi

