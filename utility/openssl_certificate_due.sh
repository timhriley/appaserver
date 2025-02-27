:
# $APPASERVER_HOME/utility/openssl_certificate_due.sh
# -------------------------------------------------------------
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 url days_threshold" 1>&2
	exit 1
fi

url=$1
days_threshold=$2

expire_days=`openssl_certificate_expire_days.sh $url`

if [ "$expire_days" -lt "$days_threshold" ]
then
	echo "yes"
else
	echo "no"
fi

exit 0
