:
# $APPASERVER_HOME/utility/openssl_certificate.sh
# -------------------------------------------------------------
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 url" 1>&2
	exit 1
fi

url=$1

from="-----BEGIN CERTIFICATE-----"
to="-----END CERTIFICATE-----"

echo									|
openssl s_client -connect ${url}:443 -servername $url 2>/dev/null	|
sed --quiet "/${from}/,/${to}/p"					|
openssl x509 -text -noout						|
cat

exit 0
