:
find . -name '*_[1-9][0-9]*' 2>/dev/null	|
grep '/appaserver/'				|
sed 's/^/rm /'					|
cat

exit 0
