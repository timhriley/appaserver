:

sed 's/CHARSET=utf8mb./CHARSET=utf8/'		|
sed 's/COLLATE=utf8mb.*;/COLLATE=utf8;/'	|
cat

exit 0
