:
# last_login_finger.sh
# --------------------

while read username
do
	echo $username
	finger $username		|
	head -3				|
	tail -1				|
	sed 's/.*/	&/g'
done

exit 0
