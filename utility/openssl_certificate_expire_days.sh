:
# $APPASERVER_HOME/utility/openssl_certificate_expire_days.sh
# -------------------------------------------------------------
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 url" 1>&2
	exit 1
fi

url=$1

days_between.sh `now.sh ymd` `openssl_certificate_renew_date.sh $url`

exit 0
