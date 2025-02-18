:
# ps2pdf.sh
# ---------------------------------------------------------------
# This is a wrapper around ps2pdf or distill.
#
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

#echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 /path/ps_file.ps /path/pdf_file.pdf" 1>&2
	exit 1
fi

# Input
# -----
ps_file=$1
pdf_file=$2
fonts_directory=/var/lib/ghostscript/fonts

if [ ! -f $ps_file ]
then
	echo "File not found error in $0: $ps_file" 1>&2
	exit 1
fi

if [ ! -d $fonts_directory ]
then
	echo "Directory not found error in $0: $fonts_directory" 1>&2
	exit 1
fi

cd $fonts_directory

ps2pdf $ps_file $pdf_file

exit $?

