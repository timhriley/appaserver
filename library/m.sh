:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_prompt_insert_form post_prompt_insert_form output_insert_table_form
fi

