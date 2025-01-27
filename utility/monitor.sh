:
# $APPASERVER_HOME/utility/monitor.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

default_delay_seconds=8

if [ "$#" -eq 1 ]
then
	delay_seconds=$1
else
	delay_seconds=$default_delay_seconds
fi

appaserver_error_directory=`appaserver_error_directory`
cd $appaserver_error_directory

while [ true ]
do
	# Top
	# ---
	clear
	top -b | head -20 | trim.e
	sleep $delay_seconds

	# Last touched error files
	# ------------------------
	clear
	results=`date '+%F %H:%M'`
	echo "$results <-- current time"

	ls -lt *.err				|
	column.e 4,5,6,7,8			|
	tr ' ' '|'				|
	piece_shift_left.e '|'			|
	tr '|' ' '				|
	head -20
	sleep $delay_seconds

	# Tail of very last touched error file
	# ------------------------------------
	last_file=`ls -t1 *.err | head -1`
	tail -20 $last_file
	sleep $delay_seconds

	# System processes
	# ----------------
	clear
	echo "System processes"

	ps -ef | tail
	sleep $delay_seconds

	# Filesystem
	# ----------
	clear
	df.sh
	sleep $delay_seconds

	# System messages
	# ---------------
	clear
	echo "System messages"

	if [ -r /var/log/messages ]
	then
		tail -10 /var/log/messages
	else
		tail -10 /var/log/syslog
	fi

	sleep $delay_seconds

	# MySQL log
	# ---------
	clear
	echo "MySQL log"
	tail -10 /var/log/mysql/error.log
	sleep $delay_seconds

	# Apache access log
	# -----------------
	clear
	echo "Apache access log"
	tail -5 /var/log/apache2/access.log
	sleep $delay_seconds

	# Apache error log
	# ----------------
	clear
	echo "Apache error log"
	tail -5 /var/log/apache2/error.log
	sleep $delay_seconds

	# Authenticate log
	# ----------------
	clear
	echo "Authenticate log"
	tail -10 /var/log/auth.log
	sleep $delay_seconds

	# Mail log
	# --------
	clear
	echo "Mail log"
	tail -10 /var/log/mail.log
	sleep $delay_seconds

	# Who
	# ---
	clear
	echo "Who"
	who
	sleep $delay_seconds

	# All the others
	# --------------
	find /var/log -name '*.log' -mtime 1 2>/dev/null		|
	while read logfile
	do
		if [	"$logfile" = "/var/log/mail.log" -o 		\
			"$logfile" = "/var/log/apache2/access.log" -o	\
			"$logfile" = "/var/log/apache2/error.log" -o	\
			"$logfile" = "/var/log/mysql/error.log" -o	\
			"$logfile" = "/var/log/auth.log" 		]
		then
			continue
		fi

		if [ -r $logfile ]
		then
			clear
			echo "$logfile"
			tail -10 $logfile
			sleep $delay_seconds
		fi

	done
done

exit 0
