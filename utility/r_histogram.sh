:
# $APPASERVER_HOME/utility/R_histogram.sh
# --------------------------------------------------------------
# No warranty and freely available software. See: appaserver.org
# --------------------------------------------------------------

echo $0 $* 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 output_pdf title footer" 1>&2
	exit 1
fi

output_pdf=$1
title="$2"
footer="$3"

tmp_file="/tmp/r_histogram_$$.dat"
echo "heading" > $tmp_file
chmod go-rw $tmp_file

cat >> $tmp_file

R --quiet --no-save --slave << --done
	table = read.csv( file="$tmp_file", header=T )
	pdf( file="$output_pdf" )
	hist( table\$heading, xlab="$footer", main="$title" )
	hist( table\$heading, xlab="$footer", main="$title", freq=F )
	quit()
--done

rm $tmp_file

exit 0
