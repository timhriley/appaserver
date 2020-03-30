:
input=hospital_load.csv

field="hospital_name,street_address,city,state_code,zip_code,zip4,telephone,regular_bed_capacity,active_yn,hospital_type,ownership_structure,helipad_yn,latitude,longitude"

table="hospital"

cat $input								|
grep -v '^NAME'								|
piece_quote_comma.e 0,1,2,3,4,5,6,17,8,7,16,18,12,13 '|'		|
sed 's/|NOT AVAILABLE|/||/g'						|
sed 's/|OPEN|/|y|/'							|
sed 's/|CLOSED|/|n|/'							|
sed 's/|-999|/||/g'							|
sed 's/|-999$/|/'							|
insert_statement.e table=$table field=$field delimiter='|'		|
cat

exit 0
