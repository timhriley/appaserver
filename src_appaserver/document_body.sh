#!/bin/sh
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/document_body.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

cat << all_done
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<link rel=stylesheet type="text/css" href=/appaserver/template/style.css>
<SCRIPT language=JavaScript1.2 src="/appaserver_home/javascript/timlib.js"></SCRIPT>
</head>

<body onload="timlib_wait_over()">
all_done

exit 0
