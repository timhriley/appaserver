:
ifconfig			|
grep 'inet addr'		|
piece.e ':' 1			|
column.e 0			|
grep -v '^127'			|
cat
