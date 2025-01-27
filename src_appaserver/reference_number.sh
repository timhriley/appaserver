:
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/reference_number.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

#echo "Starting: $0 $*" 1>&2

if [ $# -lt 2 ]
then
	echo "Usage: $0 application how_many [noupdate]" 1>&2
	exit 1
fi

application=$1
how_many=$2

export DATABASE=$application

if [ $# -eq 3 -a "$3" = "noupdate" ]
then
	noupdate=yes
fi

table_name=appaserver_application

reference_number=`echo "	select next_reference_number		 \
				from $table_name			 \
				where application = '$application';"	|\
		  sql.e`

if [ "$reference_number" = "" ]
then
	reference_number=1
fi

if [ "$noupdate" != "yes" ]
then
	next_reference_number=`expr "$reference_number + $how_many"`
	echo "	update $table_name				 	 \
		set next_reference_number = $next_reference_number	 \
		where application = '$application';"		 	 |
	sql.e
fi

echo $reference_number

exit 0
