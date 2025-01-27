:
# --------------------------------------
# $APPASERVER_HOME/utility/ufw_reset.sh
# --------------------------------------

sudo ufw disable
echo 'y' | sudo ufw reset >/dev/null
echo 'y' | sudo ufw enable >/dev/null

exit 0
