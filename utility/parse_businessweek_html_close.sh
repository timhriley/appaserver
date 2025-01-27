:
# parse_businessweek_html_close.sh
# --------------------------------

grep '<td class="tdt"'							|
egrep 'align="right"|align="center"'					|
sed 's/<td class="tdt" align="right">//'				|
sed 's/<tr class="r[12]style"><td class="tdt" align="center">//'	|
sed 's/<\/td>//'							|
joinlines.e '|' 6							|
piece.e '|' 0,3								|
date_convert.e '|' 0 international					|
cat
