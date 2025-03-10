:

make

if [ "$?" -eq 0 ]
then
	cd ../src_system
	make delete_application
fi

