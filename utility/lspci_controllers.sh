:
lspci | piece.e ':' 1 | sed 's/^[0-z][0-z]\.[0-z] //g' | sort -u

exit 0
