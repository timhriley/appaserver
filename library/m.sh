:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make cubic_feet_per_second_moving_sum
fi

