:
# ---------------------------------------
# $APPASERVER_HOME/utility/random_word.sh
# ---------------------------------------

word_file="$APPASERVER_HOME/utility/positive-words.txt.gz"

if [ ! -r $word_file ]
then
	echo "ERROR in $0: words file not found." 1>&2
	exit 0
fi

word_file_length=`zcat $word_file | grep '^[a-z]' | wc -l`

random_number=`random.e ${word_file_length}`

word=`	zcat $word_file			|
	grep '^[a-z]'			|
	head -$random_number		|
	tail -1`

echo $word

exit 0
