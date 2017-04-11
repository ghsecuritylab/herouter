<html>
<head>
<title>Virtual Settings</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

var MAX_RULES = 32;
var rules_num = <% getPortTriggerRuleNumsASP(); %> ;

function deleteClick()
{
    return true;
}

function checkRange(str, num, min, max)
{
    d = atoi(str,num);
    if(d > max || d < min)
        return false;
    return true;
}


function atoi(str, num)
{
	i=1;
	if(num != 1 ){
		while (i != num && str.length != 0){
			if(str.charAt(0) == '.'){
				i++;
			}
			str = str.substring(1);
		}
	  	if(i != num )
			return -1;
	}
	
	for(i=0; i<str.length; i++){
		if(str.charAt(i) == '.'){
			str = str.substring(0, i);
			break;
		}
	}
	if(str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++){
	    if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}

function formCheck()
{
	if(rules_num >= (MAX_RULES) ){
		alert("The rule number is exceeded "+ MAX_RULES +".");
		return false;
	}
               
	if(!document.portTrigger.portTriggerEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if(	document.portTrigger.triggerPortProtocol.value == "" &&
		document.portTrigger.triggerPortNumber.value   == "" &&
		document.portTrigger.incomingPortProtocol.value == "" &&
		document.portTrigger.incomingPortNumber.value   == "" &&
		document.portTrigger.comment.value  == "")
		alert("all empty");
		return true;

	// exam Port


	d1 = atoi(document.portTrigger.triggerPortNumber.value, 1);
	if(d1 > 65535 || d1 < 1){
		alert("Invalid port number!");
		document.portForward.fromPort.focus();
		return false;
	}
	
	d2 = atoi(document.portTrigger.incomingPortNumber.value, 1);
	if(d2 > 65535 || d2 < 1){
		alert("Invalid port number!");
		document.portForward.fromPort.focus();
		return false;
	}
   return true;
}

function display_on()
{
  if(window.XMLHttpRequest){ // Mozilla, Firefox, Safari,...
    return "table-row";
  } else if(window.ActiveXObject){ // IE
    return "block";
  }
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function initTranslation()
{
	var e = document.getElementById("portTriggerTitle");
	e.innerHTML = _("port trigger title");
	e = document.getElementById("portTriggerIntroduction");
	e.innerHTML = _("port trigger introduction");
	

	
	e = document.getElementById("triggerSetting");
	e.innerHTML = _("trigger setting");
	
		e = document.getElementById("portTriggerSettingSrv");
	e.innerHTML = _("port trigger setting server");
	
	
	e = document.getElementById("portTriggerDisable");
	e.innerHTML = _("firewall disable");
	e = document.getElementById("portTriggerEnable");
	e.innerHTML = _("firewall enable");
	e = document.getElementById("triggerPortProtocol");
	e.innerHTML = _("trigger port protocol");
  e= document.getElementById("triggerPortNumber");
	e.innerHTML = _("trigger port number");
	
  e= document.getElementById("incomingPortProtocol");
	e.innerHTML = _("incoming port protocol");
	
	e= document.getElementById("incomingPortNumber");
	e.innerHTML = _("incoming port number");

	e = document.getElementById("portTriggerApply");
	e.value = _("firewall apply");

    e = document.getElementById("currentPortTrigger");
	e.innerHTML = _("current port trigger");

	e = document.getElementById("currentTriggerPortProtocol");
	e.innerHTML = _("current trigger port protocol");
    e= document.getElementById("currentTriggerPortNumber");
	e.innerHTML = _("current trigger port number");
    e= document.getElementById("currentIncomingPortProtocol");
	e.innerHTML = _("current incoming port protocol");
	e= document.getElementById("currentIncomingPortNumber");
	e.innerHTML = _("current incoming port number");
	e = document.getElementById("currentTriggerComment");
	e.innerHTML = _("current trigger comment");
	e = document.getElementById("currentPortDel");
	e.value = _("firewall del select");
	e = document.getElementById("currentPortReset");
	e.value = _("firewall reset");
	
	e = document.getElementById("forwardCurrentVirtualSrvNo");
	e.innerHTML = _("firewall no");

}


function updateState()
{
	initTranslation();
    if(! rules_num ){
 		disableTextField(document.portTriggerDelete.deleteSelPortTrigger);
 		disableTextField(document.portTriggerDelete.reset);
	}else{
        enableTextField(document.portTriggerDelete.deleteSelPortTrigger);
        enableTextField(document.portTriggerDelete.reset);
	}
	
    if(document.portTrigger.portTriggerEnabled.options.selectedIndex == 1){   
		enableTextField(document.portTrigger.triggerPortProtocol);
		enableTextField(document.portTrigger.triggerPortNumber);
		enableTextField(document.portTrigger.incomingPortProtocol);
		enableTextField(document.portTrigger.incomingPortNumber);
		enableTextField(document.portTrigger.comment);
	}else{
		disableTextField(document.portTrigger.triggerPortProtocol);
		disableTextField(document.portTrigger.triggerPortNumber);
		disableTextField(document.portTrigger.incomingPortProtocol);
		disableTextField(document.portTrigger.incomingPortNumber);
		disableTextField(document.portTrigger.comment);
	}
}

</script>
</head>


  <!--     body - Port Forwarding    -->
<body onload="updateState()">
<table class="body"><tr><td>
<h1 id="portTriggerTitle">Port Trigger Setting</h1>
<% checkIfUnderBridgeModeASP(); %>
<p id="portTriggerIntroduction"> You may setup Port Triiger to provide services on Internet.</p>
<hr />

<form method=post name="portTrigger" action=/goform/portTrigger>
<table width="400" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="portTriggerSettingSrv">Port Trigger</td>
</tr>


<tr>
	<td class="head" id="triggerSetting">Port Trigger
	</td>
	<td>
	<select onChange="updateState()" name="portTriggerEnabled" size="1">
	<option value=0 <% getPortTriggerEnableASP(0); %> id="portTriggerDisable">Disable</option>
    <option value=1 <% getPortTriggerEnableASP(1); %> id="portTriggerEnable">Enable</option>
    </select>
    </td>
</tr>


<tr>
	<td class="head" id="triggerPortProtocol">Trigger Port Protocol
	</td>
	<td>
		<select name="triggerPortProtocol">
			<option value="TCP">TCP</option>
   		<option value="UDP">UDP</option>
   		</select>&nbsp;&nbsp;
	</td>
</tr>

<tr>
	<td class="head" id="triggerPortNumber">
		Trigger Port
	</td>
	<td>
  		<input type="text" size="5" name="triggerPortNumber"> 
	</td>
</tr>

<tr>
	<td class="head" id="incomingPortProtocol">
		Incoming Port Protocol
	</td>
	<td>
		<select name="incomingPortProtocol">
			<option  value="TCP">TCP</option>
   		<option value="UDP">UDP</option>
   		</select>&nbsp;&nbsp;
	</td>
</tr>

<tr>
	<td class="head" id="incomingPortNumber">
		Incoming Port
	</td>
	<td>
  		<input type="text" size="5" name="incomingPortNumber"> 
	</td>
</tr>
<tr>
	<td class="head" id="currentTriggerComment">
		Comment
	</td>
	<td>
		<input type="text" name="comment" size="16" maxlength="32">
	</td>
</tr>
</table>
<script>
    document.write("(The maximum rule count is "+ MAX_RULES +".)");
</script>
<p>
	<input type="submit" value="Apply" id="portTriggerApply" name="addFilterPort" onClick="return formCheck()"> &nbsp;&nbsp;
	<input type="reset" value="Reset" id="currentPortReset" name="reset">
</p>
</form>

<br>
<hr />
<!--  delete rules -->
<form action=/goform/portTriggerDelete method=POST name="portTriggerDelete">

<table width="400" border="1" cellpadding="2" cellspacing="1">	
	<tr>
		<td class="title" colspan="6" id="currentPortTrigger">Current Port Trigger in system: </td>
	</tr>

	<tr>
		<td id="forwardCurrentVirtualSrvNo"> No.</td>
		<td align=center id="currentTriggerPortProtocol"> Trigger Protocol</td>
		<td align=center id="currentTriggerPortNumber"> Trigger Port</td>
		<td align=center id="currentIncomingPortProtocol"> Incoming Protocol</td>
		<td align=center id="currentIncomingPortNumber"> Incoming port</td>
		<td align=center id="currentTriggerComment"> Comment</td>
	</tr>

	<% showPortTriggerRulesASP(); %>
</table>
<br>

<input type="submit" value="Delete Selected" id="currentPortDel" name="deleteSelPortTrigger" onClick="return deleteClick()">&nbsp;&nbsp;
<input type="reset" value="Reset" id="currentPortReset" name="reset">
</form>

<br>
			<!-----    end 2   ----->

</td></tr></table>
</body>
</html>
