:
input=hospital_load.csv

field="hospital_type"
table="hospital_type"

cat $input							|
grep -v '^NAME'							|
piece_quote_comma.e 7						|
sort -u								|
insert_statement.e table=$table field=$field delimiter=,	|
cat

exit 0
