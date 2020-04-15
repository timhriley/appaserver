:

if [ $# -eq 1 ]
then
	resume_sql=$1
fi

for file in xa*.sql
do
	if [ "$resume_sql" != "" ]
	then
		results=`strcmp.e $file $resume_sql`

		if [ $results -lt 0 ]
		then
			continue
		fi
	fi

	echo "$file"

	cat $file			|
	sed "s/'0000-00-00'/null/g"	|
	sql.e 2>&1			|
	grep -vi duplicate
done

exit 0
