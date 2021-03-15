:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make generic_daily_moving_average
fi

