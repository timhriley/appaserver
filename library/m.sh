:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_prompt_insert output_table_insert post_table_insert output_table_edit
fi

