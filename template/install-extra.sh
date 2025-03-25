:
# --------------------------------------------------------------------------
# This script is mostly reminders of the extra tasks we perform at Appahost.
# --------------------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# --------------------------------------------------------------------------

echo "# NFS isn't nececssary for Appaserver. We use this GID at Appahost."
echo "# -----------------------------------------------------------------"
echo "sudo addgroup --gid 2049 nfs"
echo "sudo usermod -a -G nfs `whoami`"

echo ""
echo "# To have MySQL use additional directories."
echo "# -----------------------------------------"
echo "$ sudo vi /etc/apparmor.d/local/usr.sbin.mysqld"
echo "vi> /ssd1/mysql r,"
echo "    /ssd1/mysql/** lrwk,"
echo ""
echo "$ sudo service apparmor restart"

echo ""
echo "# To engage Appaserver with TLS."
echo "# ------------------------------"
echo "Enable Apache site: $APPASERVER_HOME/template/appaserver.conf"

echo ""
echo "# MySQL no longer accepts a 'group by' clause of a non-selected column."
echo "# ---------------------------------------------------------------------"
echo "$ sudo vi /etc/mysql/mysql.conf.d/mysqld.cnf"
echo 'vi> sql_mode="STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_VALUE_ON_ZERO,NO_ENGINE_SUBSTITUTION"'
echo "vi> # Missing are ONLY_FULL_GROUP_BY,NO_ZERO_IN_DATE,NO_ZERO_DATE"
echo ""
echo "$ sudo service mysql restart"

echo ""
echo "# If new Linux users are to have the bash shell:"
echo "# ----------------------------------------------"
echo "$ sudo vi /etc/default/useradd"
echo "vi> SHELL=/bin/bash"

echo ""
echo "# By default, PHP generates too many messages to the log file."
echo "# ------------------------------------------------------------"
echo "$ sudo vi /etc/php/apache2/php.ini"
echo "vi> error_reporting = E_STRICT"

echo ""
echo "# If scp is to turn on the group's write bit."
echo "# -------------------------------------------"
echo 'sudo sh -c "echo session optional pam_umask.so umask=0002 >> /etc/pam.d/sshd"'
echo 'sudo sh -c "echo session optional pam_umask.so umask=0002 >> /etc/pam.d/login"'

exit 0
