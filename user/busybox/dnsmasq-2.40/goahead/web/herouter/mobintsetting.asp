<!doctype html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>无标题文档</title>
<link href="css/mainstyle.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="js/jquery.js"></script>
<script type="text/javascript" src="js/herouter.js"></script>
<style type="text/css">
<!--
body{ background:#fcfcfa url(images/fullbg.gif) repeat-x;}
.setting{ width:290px; padding:10px;}
.setting h1{ height:45px; border-bottom:1px #d3d3d3 solid;}
.setting h1 b{ line-height:45px; font-size:18px; color:#676767; float:left;}
.setting h1 span{ float:right; display:block; height:39px; width:39px; margin:3px; border-left:1px #d8d8d8 solid; background:url(images/mob/close.png) no-repeat center center; background-size:25px 25px; cursor:pointer;}
.setting h2{ font-weight:normal; font-size:10px; line-height:26px; color:#919191;}
.conf{ padding:15px 0 50px 0;  border-bottom:1px #d3d3d3 solid;}
.conf ul{}
.conf li{ font-size:15px; color:#666; line-height:36px;}
.conf li a{ text-decoration:none;}
.conf li span{ height:45px; display:block; background:#29bae6; color:#fff; line-height:45px; text-align:center; font-size:18px; cursor:pointer; }
.conf .or{ font-size:24px; line-height:60px; color:#29bae6;}
.skip{ line-height:46px;}

-->
</style>
<script type="text/javascript">
var isWanPortPlug = "<% getWanPortStatus(); %>";
var PingStatus = "<% getPingStatus(); %>";
var Mode= "<% getCfgGeneral(1, "wanConnectionMode"); %>";
function setDHCPMode()
{
	document.mobwanset.hiddenwantype.value = "DHCP";
	document.mobwanset.submit(); 
        parent.showConfProcess();
}
$(document).ready(function(){
		var str;
          if(isWanPortPlug != "pass"){
		    str="宽带口未接入网线，当前上网方式为:"+Mode;
		  }
		  else if(PingStatus == "pass"){
		    str="当前的上网方式为："+Mode;
		  }
		  else if(PingStatus != "pass")
		  {
		    if(Mode == "PPPoE"){
			 str="您的上网方式可能为：自动获取";
			}
			else{
			
			 str="您的上网方式可能为：宽带拨号";
			
			}
		  }
		  
			$("h3").text(str);

		});
</script>
</head>

<body>
<form method=post name="mobwanset" id="mobwanset" action="/goform/MobWanSet" >
<div class="setting">
	<h1><b>上网设置</b><span onClick="javascript:parent.TINY.box.hide();"></span></h1>
    <h2>*将自动检测您的上网方式，或手动设置</h2>
    <div class="conf">
    	<ul>
        	<h3 ></h3>
            <li><span onClick="setDHCPMode()">设定为DHCP方式</span></li><input type="hidden" id="hiddenwantype" name="hiddenwantype" type="text">
            <li class="or">或</li>
            <li><a href="mobilePPPoE.asp"><span>PPPoE方式</span></a></li>
        </ul>
    </div>
</form>
</body>
</html>
