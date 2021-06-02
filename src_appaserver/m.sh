:
cd ../library
make 
if [ "$?" -eq 0 ]
then
	cd -
	make post_change_sort_order
fi

