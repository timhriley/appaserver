:
results=`mysql -V`

version=`echo $results | grep 'Distrib' | sed "s/.*Distrib //"`

if [ "$version" != "" ]
then
	echo $version | piece.e ',' 0
	exit 0
fi

version=`echo $results | sed "s/.*Ver //"`

if [ "$version" != "" ]
then
	echo $version | piece.e '-' 0
	exit 0
fi

exit 1
