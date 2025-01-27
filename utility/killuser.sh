:
# killuser.sh
# -----------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 username [reverse]" 1>&2
	exit 1
fi

username=$1

if [ "$2" = "reverse" ]
then
	sort_command="sort -n -r"
else
	sort_command="sort -n"
fi

# Don't add any pipes here because the process shows up.
# ------------------------------------------------------
process_list=`ps -fu $username`

echo "$process_list"		|
grep -v PID			|
awk '{print $2}'		|
$sort_command			|
xargs -t -n1 kill -9

echo
echo $0 completed.
