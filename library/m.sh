:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch post_edit_table_form.c
	touch output_edit_table_form.c
	make post_edit_table_form
	make output_edit_table_form
fi

