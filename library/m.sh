:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch output_table_edit.c
	make output_table_edit
fi

