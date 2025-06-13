:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_table_edit
fi

