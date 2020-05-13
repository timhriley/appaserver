:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch post_login.c
	make post_login
fi

