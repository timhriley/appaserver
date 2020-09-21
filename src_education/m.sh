:

cd ../src_predictive
make

if [ "$?" -eq 0 ]
then
	cd -
	touch paypal_upload.c
	make paypal_upload
fi
