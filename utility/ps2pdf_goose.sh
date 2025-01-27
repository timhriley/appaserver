:
# ps2pdf_goose.sh
# --------------------------------
# This script calls ps2pdf.sh on
# Tim's computer "goose".
#
# Tim Riley
# --------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 /path/ps_file.ps /path/pdf_file.pdf" 1>&2
	exit 1
fi

ps_file=$1
pdf_file=$2
distill_directory=""
distill_landscape_flag=""
ghost_script_directory=/usrb2/gs/gs5.50

ps2pdf.sh	$ps_file			\
		$pdf_file			\
		"$distill_directory"		\
		"$distill_landscape_flag"	\
		$ghost_script_directory
