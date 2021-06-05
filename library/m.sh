:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make detail output_edit_table_form
fi

