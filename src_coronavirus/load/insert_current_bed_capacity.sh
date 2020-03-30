:
input=hospital_load.csv

now=`now.sh 19`
field="hospital_name,street_address,regular_bed_capacity,date_time_greenwich"

table="current_bed_capacity"

cat $input								|
grep -v '^NAME'								|
piece_quote_comma.e 0,1,17 '|'						|
sed 's/|NOT AVAILABLE|/||/g'						|
sed 's/|OPEN|/|y|/'							|
sed 's/|CLOSED|/|n|/'							|
sed 's/|-999|/||/g'							|
sed 's/|-999$/|/'							|
sed "s/$/|$now/"							|
insert_statement.e table=$table field=$field delimiter='|'		|
cat

exit 0
