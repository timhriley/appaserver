# You must dot this file
# ----------------------

# Bourne shell
# ------------
eval `ssh-agent -s`

cd ~/.ssh

if [ $? -eq 0 ]
then

	for public_filename in `ls -1 *.pub`
	do
		public_certificate=`echo $public_filename | piece.e '.' 0`
		ssh-add ~/.ssh/$public_certificate
	done
fi

cd - >/dev/null 2>&1

# Don't exit 0
