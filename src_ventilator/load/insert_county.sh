:
input=hospital_load.csv

field="county"
table="county"

cat $input							|
grep -v '^NAME'							|
piece_quote_comma.e 10						|
sort -u								|
insert_statement.e table=$table field=$field delimiter=,	|
cat

exit 0
