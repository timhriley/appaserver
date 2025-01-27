:
# $APPASERVER_HOME/utility/tmpfile.sh
# ---------------------------------------------------------------
# No warranth and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

filename="/tmp/tmpfile_$$"
touch $filename
chmod o-r $filename
echo $filename
exit 0
