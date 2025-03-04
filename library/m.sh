:

make

if [ "$?" -eq 0 ]
then
	cd ../src_system
	make create_empty_application
fi

