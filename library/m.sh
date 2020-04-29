:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make google_map_station_process
fi

