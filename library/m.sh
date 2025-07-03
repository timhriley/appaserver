:

make

if [ "$?" -eq 0 ]
then
	cd ../src_system
	make post_merge_purge_choose
fi

