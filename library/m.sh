:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make piece_quote_comma.e
fi

