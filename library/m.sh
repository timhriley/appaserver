:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make appaserver_user_trigger
fi

