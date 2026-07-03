:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_table_insert
fi

