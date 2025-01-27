:
# view_python_library.sh
# ----------------------

if [ "$#" -eq 1 -a "$1" != "" -a "$1" != " " ]
then
	grep_for=$1
else
	grep_for=""
fi

grep_command=/usr/xpg4/bin/grep

# Translate ':' to <CR> and ignore the file '.'
# ---------------------------------------------
for dir in `echo $PYTHONPATH | tr ':' '\012' | grep -v '\.$'`
do
	cd $dir

	echo $dir
	if [ "$grep_for" = "" ]
	then
		$grep_command -e 'def ' -e 'class ' *.py
	else
		$grep_command -e 'def ' -e 'class ' *.py 	|
		$grep_command "$grep_for"
	fi
done
