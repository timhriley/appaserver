:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_login output_frameset output_choose_role output_menu
fi

