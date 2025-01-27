:
# $APPASERVER_HOME/utility/flushcache.sh
# -------------------------------------------------------------
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

sudo sync
# -------------------------------------
# Clear pagecache, dentries and inodes.
# -------------------------------------
sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
# -----------------
# Clear swap space.
# -----------------
sudo sh -c 'swapoff -a && swapon -a'

exit 0
