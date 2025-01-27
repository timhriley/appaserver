:
# more_report.sh
# --------------

if [ "$#" -ne 1 ]
then
	filename=-
else
	filename=$1
fi

clear

cat $filename				|
# ---------------			|
# Trim form feeds			|
# ---------------			|
trim_character.e 12			|
less

exit 0
