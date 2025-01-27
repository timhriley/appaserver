all: a.out

a.out: test.c
	cc test.c -DAPPASERVER_ERROR_FILE=\"${APPASERVER_ERROR_FILE}\"
