:

for file in xa*.sql
do
	echo "$file"
	cat $file | sql.e
done

exit 0
