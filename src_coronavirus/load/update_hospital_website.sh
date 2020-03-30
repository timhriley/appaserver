:
input=hospital_load.csv

key="hospital_name,street_address"
table="hospital"

cat $input							|
grep -v '^NAME'							|
piece_quote_comma.e 0,1,15 '|'					|
grep -v 'NOT AVAILABLE'						|
sed 's/|/|hospital_website|/2'					|
sed 's/|/^/g'							|
update_statement.e table=$table key=$key carrot=y		|
cat

exit 0
