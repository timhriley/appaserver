:

make

if [ "$?" -eq 0 ]
then
	cd ../src_system
	make export_process
fi

