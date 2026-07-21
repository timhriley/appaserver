:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch output_prompt_process.c
	make output_prompt_process
fi

