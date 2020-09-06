:

make transaction.o journal.o

if [ "$?" -eq 0 ]
then
	cd ../src_education
	touch enrollment.c
	make enrollment_trigger
fi
