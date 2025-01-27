:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make date_ticker.e date.e minutes_ticker.e
fi
