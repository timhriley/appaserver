:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_edit_table_form
fi

