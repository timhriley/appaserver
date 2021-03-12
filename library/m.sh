:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make generic_output_text_file
fi

