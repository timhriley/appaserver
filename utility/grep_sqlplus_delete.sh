:
# grep_sqlplus_delete.sh
# ----------------------
#
# Tim Riley
# ----------------------
grep=/usr/xpg4/bin/grep

$grep -e 'ERROR' -e 'deleted' -e 'ORA'
exit $?
