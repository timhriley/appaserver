:
# --------------------------------------
# $APPASERVER_HOME/utility/ufw_enable.sh
# --------------------------------------
# sudo ufw app list
# sudo lsof -i -nP
# sudo netstat -p

sudo ufw disable
sudo ufw logging low

sudo ufw allow from 127.0.0.1
sudo ufw allow from 192.168.1.0/24
sudo ufw allow from 10.0.0.0/24

remote_ip=`echo ${SSH_CONNECTION} | column.e 0`

if [ "$remote_ip" != "" ]
then
	sudo ufw allow from $remote_ip
fi

sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow http
sudo ufw allow https
sudo ufw allow domain
sudo ufw allow smtp	# smtp v6
sudo ufw allow 25	# smtp v4
sudo ufw allow time
sudo ufw allow ssh
sudo ufw limit ssh

ufw_http_dos_rules.sh

echo 'y' | sudo ufw enable >/dev/null

exit 0
