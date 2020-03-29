:
input=hospital_load.csv

field="city,state_code,county"
table="city"

cat $input							|
grep -v '^NAME'							|
piece_quote_comma.e 2,3,10					|
sort -u								|
insert_statement.e table=$table field=$field delimiter=,	|
cat

exit 0
