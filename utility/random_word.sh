:
# ---------------------------------------
# $APPASERVER_HOME/utility/random_word.sh
# ---------------------------------------

word_file=/usr/share/dict/words

if [ ! -r $word_file ]
then
	echo "ERROR in $0: words file not found." 1>&2
	exit 0
fi

word_file_length=`cat $word_file | wc -l`

random_number=`random.e ${word_file_length}`

word=`	cat $word_file			|
	head -$random_number		|
	tail -1				|
	piece.e "'" 0			|
	sed 's/.*/\L&/g'`

echo $word

exit 0
