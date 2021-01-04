:
make

if [ "$?" -eq 0 ]
then
	cd ../src_education
	make paypal_upload
fi

