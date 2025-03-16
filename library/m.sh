:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make lookup_statistic post_prompt_lookup
fi

