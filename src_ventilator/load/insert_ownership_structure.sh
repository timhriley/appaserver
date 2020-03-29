:
input=hospital_load.csv

field="ownership_structure"
table="ownership_structure"

cat $input							|
grep -v '^NAME'							|
piece_quote_comma.e 16						|
sort -u								|
insert_statement.e table=$table field=$field delimiter=,	|
cat

exit 0
