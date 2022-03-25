:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_menu post_choose_role
fi

