:

if [ $UID -ne 0 ]
then
	sudo -p "Restarting with sudo. Password: " bash $0 $*
	exit $?
fi

cat > /etc/init.d/rc.local <<EOL
#! /bin/bash
### BEGIN INIT INFO
# Provides:          rc.local
# Required-Start:    \$local_fs \$syslog
# Required-Stop:     \$local_fs \$syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: starts rc.local
# Description:       starts rc.local using start-stop-daemon
### END INIT INFO

# put your script here

exit 0
EOL

chmod 755 /etc/init.d/rc.local
update-rc.d rc.local defaults

exit 0
