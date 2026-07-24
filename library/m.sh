:

make

if [ "$?" -eq 0 ]
then
	cd ../src_system
	make execute_select_statement
fi

