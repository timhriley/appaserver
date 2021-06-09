:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_change_sort_order
fi

