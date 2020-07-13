:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch piece_quote_comma.c
	make piece_quote_comma.e
fi

