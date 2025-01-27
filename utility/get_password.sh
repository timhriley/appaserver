:
stty -echo
echo -n "Password: "
read password
echo
stty echo
echo "got password: $password"
