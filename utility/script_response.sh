:
# script_reponse.sh
# -----------------
#
# -- Tim Riley
# -----------------

grep=/usr/xpg4/bin/grep

cat typescript			|
$grep -e ": " -e '? '		|
grep '^[A-Z]'			|
grep -v SQL
