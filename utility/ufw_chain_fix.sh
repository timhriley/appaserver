:
# -----------------------------------------
# $APPASERVER_HOME/utility/ufw_chain_fix.sh
# -----------------------------------------
sudo ufw disable
sudo iptables -F
sudo iptables -X
sudo ip6tables -F
sudo ip6tables -X
sudo ufw enable

exit 0
