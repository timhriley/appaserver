:
# ------------------------------------------
# $APPASERVER_HOME/utility/ufw_initialize.sh
# ------------------------------------------
# sudo ufw app list
# sudo lsof -i -nP
# sudo netstat -p

ufw_disable.sh
ufw_reset.sh

sudo ufw logging off
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow http
sudo ufw allow https
sudo ufw allow domain
sudo ufw allow smtp	# smtp v6
sudo ufw allow 25	# smtp v4
sudo ufw allow time
sudo ufw limit ssh

ufw_http_dos_rules.sh
ufw_whitelist_allow.sh
ufw_enable.sh

# Takes 13-14 hours.
# ufw_blacklist_deny.sh

exit 0
