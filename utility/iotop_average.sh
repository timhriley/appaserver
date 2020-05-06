:
# $APPASERVER_HOME/utility/iotop_average.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

iotop.sh			|
grep -i 'total disk read'	|
column.e 4			|
grep -v '0.00'			|
average.e '|'			|
cat
