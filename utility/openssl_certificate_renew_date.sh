:
# $APPASERVER_HOME/utility/openssl_certificate_renew_date.sh
# -------------------------------------------------------------
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 url" 1>&2
	exit 1
fi

url=$1

openssl_certificate.sh $url		|
grep 'Not After :'			|
column.e 4,3,6				|
sed 's/ /-/g'				|
date_convert.e '^' 0 international	|
cat

exit 0
