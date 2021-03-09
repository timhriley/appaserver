:
# Freely available software. See appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 execute|noexecute" 1>&2
	exit 1
fi

execute=$1

if [ "$execute" != "execute" ]
then
	less $0
	exit 0
fi

sudo apt-get -y update

sudo apt -y install openssh-server

sudo apt -y install apache2

sudo apt -y install mysql-server

# Install certbot
# ---------------
sudo apt -y install software-properties-common
sudo add-apt-repository ppa:certbot/certbot
sudo apt-get update
sudo apt -y install python-certbot-apache

# Install gcc
# -----------
sudo apt -y install build-essential

sudo apt -y install libapache2-mpm-itk

sudo apt -y install php libapache2-mod-php php-mcrypt php-mysql

sudo apt -y install texlive-latex-base texlive-pictures texlive-latex-extra

sudo apt -y install spell

sudo apt -y install grace

sudo apt -y install mailutils

sudo apt -y install iotop

sudo apt -y install ntpdate

sudo apt -y install r-base-core

# Install cs2cs
# -------------
sudo apt -y install proj-bin

echo "Next:"
echo "$ sudo ./install_appaserver_profile.sh noexecute|execute"

exit 0
