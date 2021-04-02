:
echo "select right( permit_code, 4 ) from permits where permit_code is not null;"								|
sql								|
sort								|
group.e								|
sort -n								|
grep -v '^1|'							|
piece.e '|' 1							|
while read permit_code
do
	echo "	select permit_code				\
		from fishing_trips				\
		where permit_code like '%${permit_code}';"	|
	sql
done								|
sort -u

exit 0
