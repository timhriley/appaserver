:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_insert_table_form
fi

