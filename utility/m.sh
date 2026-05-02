:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make start_time.e stop_time.e
fi
