:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch post_prompt_process.c
	make post_prompt_process
fi

