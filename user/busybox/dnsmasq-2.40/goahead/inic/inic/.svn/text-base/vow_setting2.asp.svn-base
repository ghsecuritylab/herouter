<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Flot Examples: Real-time updates</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script type="text/javascript">
function init()
{
	var root_rate = '<% getCfg2General(1, "BW_Root"); %>';
	var grate_str = '<% getCfg2General(1, "BW_Guarantee_Rate"); %>';
	var crate_str = '<% getCfg2General(1, "BW_Maximum_Rate"); %>';
	var priority_str = '<% getCfg2General(1, "BW_Priority"); %>';
	var grate = grate_str.split(";");
	var crate = crate_str.split(";");
	var priority = priority_str.split(";");

	document.forms[0].root_rate.value = ((root_rate/1024)/1024);
	document.forms[0].g_rate1.value = ((grate[0]/1024)/1024);
	document.forms[0].ceil_rate1.value = ((crate[0]/1024)/1024);
	document.forms[0].priority1.options.selectedIndex = priority[0];
	document.forms[0].g_rate2.value = ((grate[1]/1024)/1024);
	document.forms[0].ceil_rate2.value = ((crate[1]/1024)/1024);
	document.forms[0].priority2.options.selectedIndex = priority[1];
	document.forms[0].g_rate3.value = ((grate[2]/1024)/1024);
	document.forms[0].ceil_rate3.value = ((crate[2]/1024)/1024);
	document.forms[0].priority3.options.selectedIndex = priority[2];
	document.forms[0].g_rate4.value = ((grate[3]/1024)/1024);
	document.forms[0].ceil_rate4.value = ((crate[3]/1024)/1024);
	document.forms[0].priority4.options.selectedIndex = priority[3];
}
</script>
</head>
<body onload="init();">
<form method=post action="/goform/setVoW">
<table width="540" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<input type=hidden name=interface value="rai0">
  <tr>
    <td class="title" colspan="4">
      Configuration
      <input type=hidden name="debug_mode" value="1">
    </td>
  </tr>
  <tr>
    <td class="head" colspan="4">
      Root Rate:<input type=text name="root_rate" value="<% getCfg2General(1, "BW_Root"); %>" size=3 maxlength=4>Mb
    </td>
  </tr>
  <tr>
    <td class="head">SSID</td>
    <td class="head1">Guaranteed Rate</td>
    <td class="head1">Ceil Rate</td>
    <td class="head1">Priority</td>
  </tr>
  <tr>
    <td bgcolor=#edc240><% getCfg2ToHTML(1, "SSID1"); %>
    <td><input type=text name=g_rate1 value="20" size=3 maxlength=4>Mb</td>
    <td><input type=text name=ceil_rate1 value="60" size=3 maxlength=4>Mb</td>
    <td>
      <select name="priority1" size="1">
	<option value=0>Highest</option>
	<option value=1>High</option>
	<option value=2>Middle</option>
	<option value=3>Low</option>
      </select>
    </td>
  </tr>
  <tr>
    <td bgcolor=#afd8f8><% getCfg2ToHTML(1, "SSID2"); %>
    <td><input type=text name=g_rate2 value="20" size=3 maxlength=4>Mb</td>
    <td><input type=text name=ceil_rate2 value="60" size=3 maxlength=4>Mb</td>
    <td>
      <select name="priority2" size="1">
	<option value=0>Highest</option>
	<option value=1>High</option>
	<option value=2>Middle</option>
	<option value=3>Low</option>
      </select>
    </td>
  </tr>
  <tr>
    <td bgcolor=#cb4b4b><% getCfg2ToHTML(1, "SSID3"); %>
    <td><input type=text name=g_rate3 value="20" size=3 maxlength=4>Mb</td>
    <td><input type=text name=ceil_rate3 value="60" size=3 maxlength=4>Mb</td>
    <td>
      <select name="priority3" size="1">
	<option value=0>Highest</option>
	<option value=1>High</option>
	<option value=2>Middle</option>
	<option value=3>Low</option>
      </select>
    </td>
  </tr>
  <tr>
    <td bgcolor=#4da74d><% getCfg2ToHTML(1, "SSID4"); %>
    <td><input type=text name=g_rate4 value="20" size=3 maxlength=4>Mb</td>
    <td><input type=text name=ceil_rate4 value="60" size=3 maxlength=4>Mb</td>
    <td>
      <select name="priority4" size="1">
	<option value=0>Highest</option>
	<option value=1>High</option>
	<option value=2>Middle</option>
	<option value=3>Low</option>
      </select>
    </td>
  </tr>
</table>
<table width = "540" border = "0" cellpadding = "2" cellspacing = "1">
  <tr align="center">
    <td>
      <input type=submit style="{width:120px;}" value="Apply"> &nbsp; &nbsp;
      <input type=reset  style="{width:120px;}" value="Cancel" onClick="window.location.reload()">
    </td>
  </tr>
</table>
</form>
</body>
</html>
