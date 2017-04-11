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
	var grate_str = '<% getCfg2General(1, "BW_Guarantee_Rate"); %>';
	var crate_str = '<% getCfg2General(1, "BW_Maximum_Rate"); %>';
	var priority_str = '<% getCfg2General(1, "BW_Priority"); %>';
	var grate = grate_str.split(";");
	var crate = crate_str.split(";");
	var priority = priority_str.split(";");
	var rf_off = '<% getCfg2Zero(1, "RadioOff"); %>';
	var vow_on = '<% getCfg2Zero(1, "BW_Enable"); %>';

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

	if (1*rf_off == 1)
		document.forms[1].rf_button.value = "RF ON";
	else
		document.forms[1].rf_button.value = "RF OFF";
	document.forms[1].rf_button.disabled = true; // disabled by RK temporarily
	if (1*vow_on == 1)
	{
		document.getElementById("vow_status").innerHTML = "ON";
		document.forms[1].vow_button.value = "VoW OFF";
	}
	else
	{
		document.getElementById("vow_status").innerHTML = "OFF";
		document.forms[1].vow_button.value = "VoW ON";
	}
}

function RFStatusChange(rs)
{
	if (rs == 1) {
		document.forms[1].rf_button.value = "RF OFF";
		document.forms[1].rf.value = 0;
	} else {
		document.forms[1].rf_button.value = "RF ON";
		document.forms[1].rf.value = 1;
	}
}

function VoWStatusChange(rs)
{
	if (rs == 1) {
		document.forms[1].vow_button.value = "VoW OFF";
		document.forms[1].vow.value = 0;
	} else {
		document.forms[1].vow_button.value = "VoW ON";
		document.forms[1].vow.value = 1;
	}
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
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      VoW Status: <span id="vow_status"></span>
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
    </td>
    <td>
      <input type=reset  style="{width:120px;}" value="Cancel" onClick="window.location.reload()">
</form>
    </td>
<form method=post action="/goform/setVoWRadio">
<input type=hidden name=interface value="rai0">
    <td>
      <input type="button" name="rf_button" style="{width:120px;}" value="RF ON"
      onClick="if (this.value.indexOf('OFF') >= 0) RFStatusChange(1); else RFStatusChange(0); document.forms[1].submit();"> &nbsp; &nbsp; &nbsp; &nbsp;
    </td>
      <input type=hidden name=rf value="2">
    <td>
      <input type="button" name="vow_button" style="{width:120px;}" value="VoW ON"
      onClick="if (this.value.indexOf('OFF') >= 0) VoWStatusChange(1); else VoWStatusChange(0); document.forms[1].submit();">
    </td>
      <input type=hidden name=vow value="2">
</form>
  </tr>
</table>
</body>
</html>
