:
git commit -am "`git log --oneline | head -1 | piece_inverse.e 0 ' '`"

exit $?
