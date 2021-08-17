<HTML>
<HEAD>
	<link rel=stylesheet type="text/css" href=style.css>
	<TITLE>Data For Ever Login</TITLE>
<script>
	function public_submit() {
		document.login_form.login_name.value="public"
		document.login_form.password.value="public"
		document.login_form.submit()
		return true
	}
</script>
</HEAD>

<body bgcolor=#ffffff>
	<form name="login_form" enctype="multipart/form-data"
    		method=POST action="/cgi/post_login?template">

<input type="hidden" name="filename" value="index.php">

	<table	border="0" 
		cellpadding="0" 
		cellspacing="0" 
		width="680"
		align="left"
		valign="top">
	<tr valign="top"><td colspan=2>
	<img 	src="/everglades/sfnrc_banner.gif"
			width="610"
			height="60"
			border="0"
			valign="top">
	<td>
	<img 	src="/everglades/nps_logo.gif"
			width="85"
			height="105"
			border="0">
	<tr><td align="left"><h2>DataForEVER Login</h2>
	<tr><td colspan=2><hr>
<?
$invalid_yn=$_GET['invalid_yn'];
if ( $invalid_yn == 'y' )
{
	print "
	<tr><td colspan=99>Invalid login. Please try again.</td>
	";
}
?>
	<tr><td align="right"><p>Login name:
	    <td align="center"><input type="text" name="login_name" size=50
		value="">
	<tr><td align="right"><p>Password:
	    <td align="center"><input type="password" name="password" size=50
		value="">
	<tr><td align="center" colspan=2>
				<input type="submit" value="Login">
	    			<input type="button" value="Public Login"
		 onClick="public_submit()">
	</table>
	</form>
</body>
</HTML>
