<html><head>

<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">

<script language="JavaScript" type="text/javascript">

function showRsponseText(){
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			document.getElementById("showjson").innerHTML=http_request.responseText;

		} else {
			
		}
	}
}

var http_request = false;
function StartAppRequest()
{
	//var jsonBuf;
	http_request = false;
	
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
		http_request = new XMLHttpRequest();
		if (http_request.overrideMimeType) {
			http_request.overrideMimeType('text/xml');
		}
	} else if (window.ActiveXObject) { // IE
		try {
			http_request = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
			http_request = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {}
		}
	}
	if (!http_request) {
		return false;
	}
  
  //jsonBuf = "{\"userName\":\"yudeshui\",\"msgType\":\"MSG_GET_USERINFO_REQ\",\"msgSeq\":1,\"version\":16,\"clientType\":1}"
	http_request.onreadystatechange = showRsponseText;
	http_request.open("POST", "/herouter/AppRequest", true);
	http_request.send(document.jsonrequest.jsoncontent.value);

}

</script>
</head>
<body>
<form name="jsonrequest">
<tr>
	<td><input type="text" name="jsoncontent" size="128"  value=""></td>
	<td><div onclick="javascript:StartAppRequest()"><b>提交</b></div></td>
</tr>	
</form>
<div id="showjson"></div>
</body></html>
 
