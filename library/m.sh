:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make insert_mysql
fi

