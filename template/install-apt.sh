:
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

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

# Install gcc
# -----------
sudo apt -y install build-essential

# Install cs2cs
# -------------
sudo apt -y install proj-bin

sudo apt -y install openssh-server
sudo apt -y install graphviz
sudo apt -y install texlive-latex-base
sudo apt -y install texlive-pictures texlive-latex-extra
sudo apt -y install texlive-latex-extra
sudo apt -y install spell
sudo apt -y install grace
sudo apt -y install ghostscript
sudo apt -y install mailutils
sudo apt -y install iotop
sudo apt -y install ntpdate
sudo apt -y install r-base-core
sudo apt -y install libapache2-mpm-itk
sudo apt -y install php
sudo apt -y install libapache2-mod-php
sudo apt -y install php-mysql
sudo apt -y install inotify-tools
sudo apt -y install imagemagick-6.q16

echo "Next:"
echo "$ sudo ./install-profile.sh execute|noexecute"

exit 0
