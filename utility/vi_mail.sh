:
# vi_mail.sh
# ----------

whoami=`whoami`
vi_file=/var/spool/mail/$whoami

vi $vi_file
