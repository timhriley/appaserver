:
# $APPASERVER_HOME/utility/certbot.sh
# ---------------------------------------------------------------
# No warranth and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$#" -lt 3 ]
then
	echo "Usage: $0 operation domain document_root_leaf [testing]" 1>&2
	echo "operation = {create,delete,renew}" 1>&2
	echo "Note: only run this if you're fully using the Group Centric Paradigm." 1>&2
	echo "" 1>&2
	echo "Note: The document_root_leaf is the subdirectory under document_root where the virtual host files are stored." 1>&2
	echo "" 1>&2
	echo "Note: to list certificates:" 1>&2
	echo "$ sudo certbot certificates" 1>&2
	exit 1
fi

operation=$1
domain=$2
document_root_leaf=$3

if [ "$#" -eq 4 -a "$4" = "staging" ]
then
	testing=yes
fi

if [ "$#" -eq 4 -a "$4" = "testing" ]
then
	testing=yes
fi

webroot_path="${DOCUMENT_ROOT}/${document_root_leaf}"
cert_path="/etc/letsencrypt/live/${domain}/cert.pem"
key_path="/etc/letsencrypt/archive/${domain}"

function certbot_delete ()
{
	domain=$1
	testing=$2

	if [ "$testing" = "yes" ]
	then
		staging="--dry-run"
	fi

	sudo certbot revoke --cert-path $cert_path $staging
	sudo certbot delete --cert-name $domain $staging
}

function certbot_create ()
{
	domain=$1
	document_root_leaf=$2
	testing=$3

	if [ "$testing" = "yes" ]
	then
		staging="--dry-run"
	fi

	sudo certbot	certonly			\
			--webroot			\
			--webroot-path			\
			${webroot_path}			\
			$staging			\
			-d $domain

	if [ "$testing" != "yes" ]
	then
		sudo chmod g+rwxs ${key_path}
		sudo chmod o-rx ${key_path}
		sudo chmod o-r ${key_path}/*
	fi
}

function certbot_renew ()
{
	domain=$1
	testing=$2

	if [ "$testing" = "yes" ]
	then
		staging="--dry-run"
	fi

	sudo certbot renew --cert-name $domain $staging
}

if [ "$operation" = "delete" ]
then
	certbot_delete $domain $testing
elif [ "$operation" = "create" ]
then
	certbot_create $domain $document_root_leaf $testing
elif [ "$operation" = "renew" ]
then
	certbot_renew $domain $testing
else
	echo "$0: Invalid operation." 1>&2
	exit 1
fi

exit 0
