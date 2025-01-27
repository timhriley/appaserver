:
# ----------------------------------
# $APPASERVER_HOME/utility/du_gig.sh
# ----------------------------------
if [ "$#" -eq 1 ]
then
	cd $1
fi

find . -xdev -type d -print 2>/dev/null			|
xargs.e 'du -hxs "{}" 2>/dev/null'			|
grep '^[1-9][0-9]*\.*[0-9]*G'				|
sort -nr						|
head -20

exit 0
