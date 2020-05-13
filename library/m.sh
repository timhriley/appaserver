:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch insert_mysql.c
	make insert_mysql
fi

