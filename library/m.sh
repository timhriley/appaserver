:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_edit_table_form post_prompt_edit_form
fi

