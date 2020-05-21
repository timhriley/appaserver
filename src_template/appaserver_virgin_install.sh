:
# ------------------------------------------------------------
# Execute this if on a virgin Linux install.
# Note: unlike the other scripts, there's no undo script here.
# ------------------------------------------------------------
# Freely available software. See appaserver.org
# ------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 username execute|noexecute" 1>&2
	exit 1
fi

username=$1
execute=$2

if [ "$execute" = "execute" ]
then
	# Apache may not be installed yet.
	# --------------------------------
	sudo addgroup --gid 33 www-data 2>/dev/null

	# R may not be installed yet.
	# ---------------------------
	sudo addgroup --gid 50 staff 2>/dev/null

	# Create the Appaserver user.
	# ----------------------------------------------------------------
	# The virgin install assumes you've currently running on UID=1000.
	# ----------------------------------------------------------------
	sudo addgroup --gid 1001 $username
	sudo useradd -m --shell /bin/bash --uid 1001 --gid 1001 $username
	sudo usermod -a -G sudo $username
	sudo usermod -a -G www-data $username
	sudo usermod -a -G staff $username
	sudo passwd $username

	# Create the appaserver group
	# ---------------------------
	sudo addgroup --gid 1002 appaserver
	sudo usermod -a -G appaserver $username

	# NFS isn't nececssary for Appaserver. We use this GID at Appahost.
	# -----------------------------------------------------------------
	sudo addgroup --gid 1003 nfs
	sudo usermod -a -G nfs $username
fi

echo ""
echo "Apparmor reminder"
echo "-----------------"
echo "$ sudo vi /etc/apparmor.d/local/usr.sbin.mysqld"
echo "vi>/ssd1/mysql r,"
echo "   /ssd1/mysql/** lrwk,"
echo ""
echo "$ sudo service apparmor restart"

echo ""
echo "Apache reminder"
echo "---------------"
echo "$ sudo vi /etc/apache2/apache2.conf"
echo "vi>Timeout 1500"
echo ""
echo "$ sudo vi /etc/apache2/mods-available/dir.conf"
echo "vi>DirectoryIndex index.php index.html ..."
echo ""
echo "$ sudo vi /etc/apache2/envvars"
echo "vi>umask 0002"
echo "Make sure /etc/init.d/apache2 runs /etc/apache2/envvars"
echo ""
echo "$ sudo vi /etc/apache2/mods-available/ssl.conf"
echo "vi>SSLCipherSuite AES256+EECDH:AES256+EDH:AES128+EECDH:AES128+EDH"
echo "vi>SSLProtocol -ALL -SSLv2 -SSLv3 -TLSv1 -TLSv1.1 +TLSv1.2"
echo "vi>SSLHonorCipherOrder on"
echo "vi>SSLStrictSNIVHostCheck Off"
echo "vi>SSLCompression off"
echo ""
echo "$ sudo a2ensite default-ssl"
echo "$ sudo a2enmod ssl"
echo "$ sudo service apache2 restart"
echo "$ sudo systemctl status apache2"

echo ""
echo "Mysql reminder"
echo "--------------"
echo "$ sudo vi /etc/mysql/mysql.conf.d/mysqld.cnf"
echo 'vi>sql_mode = "-- omit ONLY_FULL_GROUP_BY --"'
echo ""
echo "$ sudo service mysql restart"

echo ""
echo "Bash reminder"
echo "-------------"
echo "$ sudo vi /etc/default/useradd"
echo "vi>SHELL=/bin/bash"

echo ""
echo "PHP reminder"
echo "------------"
echo "$ sudo vi /etc/php/apache2/php.ini"
echo "vi>error_reporting = E_STRICT"

echo ""
echo "Umask reminder"
echo "--------------"
echo 'sudo sh -c "echo session optional pam_umask.so umask=002 >> /etc/pam.d/sshd"'
echo 'sudo sh -c "echo session optional pam_umask.so umask=002 >> /etc/pam.d/login"'

exit 0
