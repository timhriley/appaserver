:
# src_predictive/recovery_statutory_accelerated_half_year.sh
# ----------------------------------------------------------
# Freely available software. See appaserver.org
# ----------------------------------------------------------

if [ $# -lt 1 ]
then
	echo "Usage: $0 years [tax_year]" 1>&2
	exit 1
fi

years=$1

if [ $# -eq 2 ]
then
	tax_year=$2

	recovery_statutory_accelerated_half_year $years	|
	grep "^$tax_year|"					|
	piece.e '|' 1
else
	recovery_statutory_accelerated_half_year $years
fi

exit 0
