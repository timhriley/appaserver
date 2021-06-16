:

cd ../src_predictive
make

if [ "$?" -eq 0 ]
then
	cd -
	make enrollment_trigger
fi
