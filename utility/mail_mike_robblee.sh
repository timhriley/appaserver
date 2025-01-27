:
# mail_mike_robblee.sh
# --------------------

if [ "$#" = 1 ]
then
	mailx -s "$1" robbleem@fiu.edu
else
	mailx robbleem@fiu.edu
fi

exit $?
