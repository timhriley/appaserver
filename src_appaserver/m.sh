:
cd ../library
make 
if [ "$?" -eq 0 ]
then
	cd -
	make statistics_folder output_edit_table_form
fi

