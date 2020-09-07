:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_prompt_edit_form output_edit_table_form
fi

