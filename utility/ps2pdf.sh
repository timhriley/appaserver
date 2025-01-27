:
# ps2pdf.sh
# ---------------------------------------------
# This is a wrapper around ps2pdf or distill.
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 /path/ps_file.ps /path/pdf_file.pdf" 1>&2
	exit 1
fi

ps_file=$1
pdf_file=$2

if [ ! -f $ps_file ]
then
	echo "File not found error in $0: $ps_file" 1>&2
	exit 1
fi

#export GS_LIB
#PATH=$GS_LIB:$GS_LIB/obj:$GS_LIB/bin:$PATH;export PATH
#cd $GS_LIB/fonts 2>/dev/null

cd /var/lib/ghostscript/fonts 2>/dev/null

ps2pdf $ps_file $pdf_file

exit $?

