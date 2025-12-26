:
# $APPASERVER_HOME/src_canvass/apartment.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

piece.e '^' 0,1 			|
grep -v '^Street'			|
grep -v '^Sum'				|
grep -v '\^0$'				|
piece.e '^' 0				|
apartment_complex.sh			|
double_quote_comma_delimited.e '^'

exit 0
