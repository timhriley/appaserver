:
# --------------------------------------
# $APPASERVER_HOME/utility/ip_gateway.sh
# --------------------------------------

ip route | grep default | column.e 2

exit $?
