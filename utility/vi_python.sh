:
if [ "$#" -ne 1 ]
then
	echo "$0 import_module"
	exit 1
fi

vi /home/riley/python/lib/python1.5/$1.py
