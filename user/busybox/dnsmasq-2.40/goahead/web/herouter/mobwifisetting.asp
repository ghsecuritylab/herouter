<!doctype html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>无标题文档</title>
<link href="css/mainstyle.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="js/jquery.js"></script>
<style type="text/css">
<!--
body{ background:#fcfcfa url(images/fullbg.gif) repeat-x;}
*{-moz-transition: all 0.3s ease-in;-webkit-transition: all 0.3s ease-in;-o-transition: all 0.3s ease-in;transition: all 0.3s ease-in;}
*:hover{-moz-transition: all 0.3s ease-in;-webkit-transition: all 0.3s ease-in;-o-transition: all 0.3s ease-in;transition: all 0.3s ease-in;}
.setting{ width:290px; padding:10px;}
.setting h1{ height:45px; border-bottom:1px #d3d3d3 solid;}
.setting h1 b{ line-height:45px; font-size:18px; color:#676767; float:left;}
.setting h1 span{ float:right; display:block; height:39px; width:39px; margin:3px; border-left:1px #d8d8d8 solid; background:url(images/mob/close.png) no-repeat center center; background-size:25px 25px; cursor:pointer;}
.setting h2{ font-weight:normal; font-size:10px; line-height:26px; color:#919191;}
.conf{ padding:15px 0 15px 0; }
.tt{ height:30px; padding:10px 0;}
.tt b{ float:left; font-size:15px; color:#666; line-height:30px;}
.switch{ float:left;}
.selectswitch{ height:30px;}
.selectswitch em{ float:left; height:20px; width:20px; display:block; margin:5px; padding:0;}
.selectswitch .closestate{ background:url(images/wifistateicon.png) no-repeat;}
.selectswitch .openstate{background:url(images/wifistateicon.png) no-repeat 0 -40px;}
.selectswitch span{ float:left; padding-left:0px; background:url(images/selbg.png) no-repeat; width:55px; height:29px; display:block;}
.selectswitch strong{ display:block; margin:3px; height:23px; width:49px; background:url(images/select.png) no-repeat right center; cursor:pointer;}


.switchoff{}
.switchoff .closestate{ background-position:0 -20px;}
.switchoff .openstate{ background-position:0 -60px;}
.switchoff  strong{ background-position:left center;}
.conf ul{}
.conf li{ padding:5px 0; height:40px;}
.inp1{ height:40px; background:#fff; border:1px #c2c2c2 solid; width:280px; padding:0 5px; font-size:15px; }
.inp2{ height:40px; background:#fff; border:1px #c2c2c2 solid; width:48px; padding:0 5px; font-size:15px; }
.inp3{ height:40px; background:#fff; border:1px #c2c2c2 solid; width:215px; padding:0 5px; font-size:15px; }
.wifihide{ }
.hidenet{ float:right;background:#29bae6; padding:2px; border-radius: 5px;-webkit-border-radius: 5px; height:32px; overflow:hidden; }
.checkinp{height:26px; width:26px; padding:0; border:none; background:#f5f5f5;}
.hidenet span{ font-size:14px; color:#fff; line-height:32px; padding-right:10px;}
.submit{}
.submit strong{height:45px; display:block; background:#29bae6; color:#fff; line-height:45px; text-align:center; font-size:18px; cursor:pointer; }
-->
</style>
<script type="text/javascript">

var WIFIAuthMode = '<% getCfgGeneral(1, "AuthMode"); %>';
var wifi_off='<% getCfgZero(1, "WiFiOff"); %>';
var Key1Type='<% getCfgZero(1, "Key1Type"); %>'; 
var EncrypType='<% getCfgZero(1, "EncrypType"); %>'; 
var RekeyInterval='<% getCfgZero(1, "RekeyInterval"); %>';
var HideSSID='<% getCfgZero(1, "HideSSID"); %>'; 
var wpsenable  = '<% getCfgZero(1, "WscModeOption"); %>';
var IEEE8021X  = '<% getCfgZero(1, "IEEE8021X"); %>';
var WPAPSK1  = '<% getCfgZero(1, "WPAPSK1"); %>';
var Key1Str1  = '<% getCfgZero(1, "Key1Str1"); %>';
var wifissid = '<% getCfgToHTML(1, "SSID1"); %>';


var switchcode=wifi_off;
$(document).ready(function(){
	if(wifi_off=='0')
	{
		$("#state").removeClass("switchoff");
		$(".wifiinfo").css("display","");
		switchcode=1;
	}
	else
	{
		$("#state").addClass("switchoff");
		$(".wifiinfo").css("display","none");
		switchcode=0;
	}
	
	if(HideSSID == '1')
	{
		document.getElementById("hssid").checked = true;
	}
	else
	{
		document.getElementById("hssid").checked = false;
	}
	
	var str = new Array();
	str = wifissid.split("_"); 
	document.getElementById("wifiprefix").value = str[0] + "_";
	document.getElementById("wifiname0").value = str[1];

	var WIFIAuthModeArray = WIFIAuthMode.split(";");
	if(WIFIAuthModeArray[0] == 'OPEN')
	{
		document.getElementById("listselect").value = '不加密';
		//document.getElementById("wifikeyshow").style.display = 'none';
  document.getElementById("wifipasswd").style.display = 'none';
	}
	else
	{
		document.getElementById("listselect").value ='加密';
		document.getElementById("wifipasswd").value = WPAPSK1;
	}
	
	
	$("#switch").click(function(){
		if(switchcode=='0'){
			 $("#state").removeClass("switchoff");
			 $(".wifiinfo").css("display","");
			 switchcode=1;
			}else{
			$("#state").addClass("switchoff");
			$(".wifiinfo").css("display","none");
			switchcode=0;
			}
	})
	$('#list').change(function(){ 
	$("#listselect").val($("#list").find("option:selected").text());
	});
});

function changeshow(show)
{
	if(show =='0')
	{
	//	document.getElementById("wifikeyshow").style.display = 'none';
  document.getElementById("wifipasswd").style.display = 'none';
	}
	else
	{
//		document.getElementById("wifikeyshow").style.display = 'block';
  document.getElementById("wifipasswd").style.display = 'block';
		document.getElementById("wifipasswd").value = WPAPSK1;
	}
}

function wifi_apply()
{
	//if (check_value() == true)
	//{
	  
	  if(document.wireless_mobset.wifiname0.value.length<1)
	  {
	   alert('请输入SSID');
	   return false;
	  }
	  if(document.getElementById("listselect").value == '不加密')
	  {
	  	document.wireless_mobset.hiddentype.value = 'OPEN';
		document.wireless_mobset.wifipasswd.value = WPAPSK1;
	  }
	  else
	  {
	  	if (document.wireless_mobset.wifipasswd.value.length < 8)
		  {
				alert('请输入8位以上密码');
				return false;
		  }
	  	document.wireless_mobset.hiddentype.value = 'WPAPSKWPA2PSK';
	  }
	  
	  if(document.getElementById("hssid").checked == true)
	  {
	  	document.wireless_mobset.hssid.value = 1;
	  }
	  else
	  {
	  	document.wireless_mobset.hssid.value = 0;
	  }
	  document.wireless_mobset.wifihiddenButton.value = switchcode;
	  document.wireless_mobset.wifiname.value = "CMCC_" + document.wireless_mobset.wifiname0.value;

	  if(document.getElementById("listselect").value == '加密')
		  document.getElementById("listselect").value ="WPAPSKWPA2PSK";

      document.wireless_mobset.submit();  
if(document.getElementById("listselect").value =="WPAPSKWPA2PSK")
		  document.getElementById("listselect").value ="加密";

 parent.showConfProcess();
	//}
}
function tt(id) {
  var aa = document.getElementById(id);
  if (aa.value=="加密")
  {
  document.getElementById("wifipasswd").style.display = 'block';
  }
  else
  {
  document.getElementById("wifipasswd").style.display = 'none';
  }
}
</script>
</head>

<body>
<form method="post" name="wireless_mobset" id="wireless_mobset" action="/goform/wirelessmobset">
<div class="setting">
	<h1><b>上网设置</b><span onClick="javascript:parent.TINY.box.hide();"></span></h1>
    <h2>*移动无线设备接入以下“网络名称”上网</h2>
    <div class="conf">
    	<div class="tt">
        	<b>无线网络开关</b>
            <div class="switch">
            	<div class="selectswitch" id="state">
					<em class="closestate"></em><span><strong id="switch"></strong></span><em class="openstate"></em>
					<input type="hidden" id="wifihiddenButton" name="wifihiddenButton" value="">
				</div>
            </div>
        </div>
		<div class="wifiinfo">
			<ul>
				<li style="display: none;"><input class="inp1" name="wifiname"  id="wifiname" type="text"></li>
			  <li><div style=" position:absolute;">
			  	<input class="inp2" disabled="disabled" name="wifiprefix"  id="wifiprefix" type="text" value="CMCC_">
			  	<input class="inp3" name="wifiname0"  id="wifiname0" type="text" placeholder="网络名称" onkeyup="value=value.replace(/[^\w\.\/]/ig,'')">
			  </div></li>
			  <li class="wifihide"><div class="hidenet"><input  id="hssid" name="hssid" class="checkinp" type="checkbox" value=""><span>隐藏网络</span></div> </li>
			  <li><div style=" position:absolute;"><select id="list"  onchange="tt(this.id)" name="telewell" style="height:40px; width:290px;filter:alpha(opacity=0);opacity: 0;">
				<option value="不加密" onClick="changeshow(0)" >不加密</option>
				<option value="加密" onClick="changeshow(1)">加密</option>
			  </select></div><input class="inp1" id="listselect" name="listselect" type="text" placeholder="安全类型"><input type="hidden" id="hiddentype" name="hiddentype" type="text"></li>
			  <li id="wifikeyshow"><input class="inp1"  id="wifipasswd" name="wifipasswd" type="test" placeholder="网络密码"></li>
			  
				
			  
			</ul>
		</div>
		<li class="submit" onClick="javascript:wifi_apply();"><strong>保   存</strong></li>
    </div>
</div>
</form>
</body>
</html>
