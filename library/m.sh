:

make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make unescape_character.e
fi

