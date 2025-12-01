:

make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_ajax_window
fi

