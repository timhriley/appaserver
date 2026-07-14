:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_prompt_lookup
fi

