:
cd ../library
make 
if [ "$?" -eq 0 ]
then
	cd -
	make post_merge_purge
fi

