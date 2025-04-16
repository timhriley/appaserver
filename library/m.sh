:

make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make feeder_upload
fi

