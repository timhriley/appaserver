:

# key_type = {dsa | ecdsa | ecdsa-sk | ed25519 | ed25519-sk | rsa}
# ----------------------------------------------------------------
if [ "$#" -eq 1 ]
then
	key_type=$1
else
	key_type=rsa
fi

ssh-keygen -t $key_type
exit $?
