<html>
<head>
<link rel=stylesheet type="text/css" href="/appaserver_home/template/style.css">
<title>title_change_me</title>
</head>

<body bgcolor="FFFFFF" onload="document.login.login_name.focus()">

<form enctype="multipart/form-data"
name="login" method=POST action="/cgi-bin/post_login?application_change_me">

<input type="hidden" name="filename" value="index.php">

<table	border="0"
cellpadding="0"
cellspacing="0"
width="540"
align="center"
valign="top">
<tr><td align="center">
	<img src="/zimages/logo_appaserver.jpg" border=1>
	<tr><td align="center">
	<font color="blue">
	<h1>title_change_me</h1>
	<tr><td><hr>
	<tr><td>
	<table align=center>

<?php

$emailed_login_yn=$_GET['emailed_login_yn'];
if ( $emailed_login_yn == 'y' )
{
	print "
	<tr><td colspan=99 align=center>
	<font color=blue>
	A link to your session was emailed to you.</td>
	";
}

$invalid_login_yn=$_GET['invalid_login_yn'];
if ( $invalid_login_yn == 'y' )
{
	print "
	<tr><td colspan=99 align=center>
	<font color=red>
	Invalid login. Please try again.</td>
	";
}

$invalid_password_yn=$_GET['invalid_password_yn'];
if ( $invalid_password_yn == 'y' )
{
	print "
	<tr><td colspan=99 align=center>
	<font color=red>
The password was incorrect. Please try again.</td>
	";
}

?>
	<tr><td align="right">
	<font color="blue">
	Login name
	<td align="left">
	<input type="text" name="login_name" size=20 maxlength=50 value="">
	<tr><td align="right">
	<font color="blue">
	Password
	<td align="left">
	<input type="password" name="password" size=20 maxlength=50 value="">
	</table>
	<tr><td align="center">
	<font color="blue">
	<input type="submit" value="Login">
</table>
</form>
</body>
</html>
