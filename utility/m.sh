:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make mysql_remove_null.e
fi
