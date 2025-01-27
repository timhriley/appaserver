:
for filename in *
do
	without_underbar=`echo $filename | sed 's/[ ]/_/g'`
	mv "$filename" $without_underbar 
done
exit 0

