:
if [ $# -ne 1 ]
then
	echo "Usage: $0 version.release" 1>&2
	exit 1
fi

version=$1

sudo service mysqld stop
sudo killall -9 mysql
sudo killall -9 mysqld
sudo apt-get remove --purge mysql-server mysql-client mysql-common
sudo apt-get autoremove
sudo apt-get autoclean
sudo deluser mysql
sudo delgroup mysql
sudo rm -rf /var/lib/mysql
sudo apt-get purge mysql-server-core-$version
sudo apt-get purge mysql-client-core-$version
sudo rm -rf /var/log/mysql
sudo rm -rf /etc/mysql
sudo rm -rf /var/log/mysql
sudo rm -rf /etc/mysql

exit 0
