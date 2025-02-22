:

make

if [ "$?" -eq 0 ]
then
	cd ../src_system
	make fix_orphans
fi

