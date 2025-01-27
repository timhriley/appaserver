:
# mail_riley.sh
# -------------

if [ "$#" = 1 ]
then
	mailx -s "$1" riley
else
	mailx riley
fi

exit $?
