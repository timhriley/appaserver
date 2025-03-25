:

sed 's/CHARSET=utf8mb./CHARSET=utf8/'			|
sed 's/COLLATE=utf8mb.*;/COLLATE=utf8_general_ci;/'	|
cat

exit 0
