:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch post_prompt_insert.c
	make post_prompt_insert
fi

