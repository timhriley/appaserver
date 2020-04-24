:
# ---------------------------------------------------
# $APPASERVER_HOME/src_appaserver/application_list.sh
# ---------------------------------------------------

dir=`get_appaserver_error_directory`

ls $dir/appaserver_*.err		|
sed "s|^$dir/||"			|
sed 's/appaserver_//'			|
sed 's/\.err$//'			|
cat

exit 0
