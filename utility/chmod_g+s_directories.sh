:

find . -type d -exec chmod g+rwxs {} 2>&1 \; | grep -v "Operation not permitted"

exit 0
