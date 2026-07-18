:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch output_prompt_lookup.c
	make output_prompt_lookup
fi

