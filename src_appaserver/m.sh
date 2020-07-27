:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make export_subschema
fi

