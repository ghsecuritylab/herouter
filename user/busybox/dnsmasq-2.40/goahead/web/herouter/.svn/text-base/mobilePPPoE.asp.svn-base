<!doctype html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>无标题文档</title>
<link href="css/mainstyle.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="js/jquery.js"></script>
<style type="text/css">
<!--
body{ background:#fcfcfa url(images/fullbg.gif) repeat-x;}
.setting{ width:290px; padding:10px;}
.setting h1{ height:45px; border-bottom:1px #d3d3d3 solid;}
.setting h1 b{ line-height:45px; font-size:18px; color:#676767; float:left;}
.setting h1 span{ float:right; display:block; height:39px; width:39px; margin:3px; border-left:1px #d8d8d8 solid; background:url(images/mob/close.png) no-repeat center center; background-size:25px 25px; cursor:pointer;}
.setting h2{ font-weight:normal; font-size:10px; line-height:26px; color:#919191;}
.conf ul{}
.conf li{ padding:5px 0; height:40px;}
.inp1{ height:40px; background:#fff; border:1px #c2c2c2 solid; width:280px; padding:0 5px; font-size:15px; }
.skip{ line-height:46px;}
.hidenet{ float:right;background:#29bae6; padding:2px; -moz-border-radius: 5px;border-radius: 5px;-webkit-border-radius: 5px; height:32px; overflow:hidden; }
.checkinp{height:26px; width:26px; padding:0; border:none; background:#f5f5f5;}
.hidenet span{ font-size:14px; color:#fff; line-height:32px; padding-right:10px;}
.submit{}
.submit strong{height:45px; display:block; background:#29bae6; color:#fff; line-height:45px; text-align:center; font-size:18px; cursor:pointer; }
-->
</style>
</head>
<script type="text/javascript">
var usr='<% getCfgGeneral(1, "wan_pppoe_user"); %>';
var pas='<% getCfgGeneral(1, "wan_pppoe_pass"); %>';

$(document).ready(function(){
var $password = $('#password');
var	$passwordInput = $('<input type="text" name="' + $password.attr('name') + '" class="' + $password.attr('class') + '" />');
document.getElementById("username").value = usr;
document.getElementById("password").value = pas;
});
$('#showpasswd').click(function(){
		if(this.checked){
			$password.replaceWith($passwordInput.val($password.val()));
		}else{
			$passwordInput.replaceWith($password.val($passwordInput.val()));
		}
	});
var g_usrFlag = 1;
var g_pssFlag = 1;
function clsusr(){
	if (g_usrFlag == 1)
	{
		document.getElementById("username").value = "";
		g_usrFlag = 0;
	}
}

function clspas(){
	if (g_pssFlag == 1)
	{
		document.getElementById("password").value = "";
		g_pssFlag = 0;
	}
}
function pppoe_apply()
{
	document.mobpppoeset.submit();
  // if(confirm("请再次确定是否连接？"))
    parent.showConfProcess();
}
</script>

<body>
<form method=post name="mobpppoeset" id="mobpppoeset" action="/goform/MobPppoeSet" >
<div class="setting">
	<h1><b>拨号设置</b><span onClick="javascript:parent.TINY.box.hide();"></span></h1>
    <h2>*使用PPPoE拨号方式连接互联网</h2>
    <div class="conf">
    	 <ul>
       	  <li><input class="inp1" name="username" id="username" type="text" placeholder="账号" onFocus="clsusr()"></li>
          <li><input class="inp1" id="password" name="password" type="text" placeholder="密码" onFocus="clspas()"></li>
          <!--li class="wifihide"><div class="hidenet"><input name="showpasswd" id="showpasswd" class="checkinp" type="checkbox" value=""><span>显示密码</span></div> </li-->
          <li class="submit" onClick="javascript:pppoe_apply();"><strong>连 接</strong></li>
        </ul>
    </div>
    <div class="skip"><a href="#">跳过上网设置 >></a></div>
</div>
</form>
</body>
</html>
