:
whoami=`whoami`
who_me=`who | grep "^$whoami "`
column_4=`echo $who_me | column.e 4`
piece_1=`echo $column_4 | piece.e '(' 1`
current_ip_address=`echo $piece_1 | piece.e '.' 0 | sed 's/-/./g'`

echo $current_ip_address

exit 0
