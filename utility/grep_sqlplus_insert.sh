:
# grep_sqlplus_insert.sh
# ----------------------
#
# Tim Riley
# ----------------------
grep=/usr/xpg4/bin/grep

$grep -e 'ERROR' -e 'created' -e 'ORA'
exit $?
