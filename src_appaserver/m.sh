:
cd ../library
make 
if [ "$?" -eq 0 ]
then
	cd -
	make output_edit_table_form
fi

