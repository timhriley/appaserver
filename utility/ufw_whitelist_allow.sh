:
# -----------------------------------------------
# $APPASERVER_HOME/utility/ufw_whitelist_allow.sh
# -----------------------------------------------

ufw_whitelist.e					|
xargs.e sudo ufw insert 1 allow from {}

exit 0
