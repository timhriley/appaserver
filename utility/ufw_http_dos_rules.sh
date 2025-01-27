:
# ----------------------------------------------
# $APPASERVER_HOME/utility/ufw_http_dos_rules.sh
# ----------------------------------------------
# Test with "siege $server"
# ----------------------------------------------

rules="/etc/ufw/before.rules"

# http://blog.lavoie.sl/2012/09/protect-webserver-against-dos-attacks.html
# ------------------------------------------------------------------------
insert_http_identifiers()
{
temp="/tmp/ufw_$$"

# Will append a new commit later.
# -------------------------------
cat $rules			|
sed 's/^COMMIT/#COMMIT/' > $temp
cat $temp > $rules

cat $rules			|
sed 's/^*filter/&\n:ufw-http - [0:0]\n:ufw-http-logdrop - [0:0]/' > $temp
cat $temp > $rules

rm -f $temp
}

append_http_limit()
{

cat >> $rules << all_done1

### Start HTTP ###

# Enter rule
-A ufw-before-input -p tcp --dport 80 -j ufw-http
-A ufw-before-input -p tcp --dport 443 -j ufw-http

# Limit connections per Class C
-A ufw-http -p tcp --syn -m connlimit --connlimit-above 50 --connlimit-mask 24 -j ufw-http-logdrop

# Limit connections per IP
-A ufw-http -m state --state NEW -m recent --name conn_per_ip --set
-A ufw-http -m state --state NEW -m recent --name conn_per_ip --update --seconds 10 --hitcount 20 -j ufw-http-logdrop

# Limit packets per IP
-A ufw-http -m recent --name pack_per_ip --set
-A ufw-http -m recent --name pack_per_ip --update --seconds 1 --hitcount 20 -j ufw-http-logdrop

# Finally accept
-A ufw-http -j ACCEPT

-A ufw-http-logdrop -m limit --limit 3/min --limit-burst 10 -j LOG --log-prefix "[UFW HTTP DROP] "
-A ufw-http-logdrop -j DROP

### End HTTP ###

COMMIT

all_done1

}

rules_file_no_write()
{
	sudo chown root $rules
}

rules_file_write()
{
	sudo chown `whoami` $rules
}

rules_file_write
cp $rules /tmp
insert_http_identifiers
append_http_limit
rules_file_no_write

exit 0
