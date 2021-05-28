:
cd ../library
make 
if [ "$?" -eq 0 ]
then
	cd -
	make post_edit_table_form
fi

