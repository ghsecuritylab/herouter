<!doctype html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>互联网设置</title>
<link href="css/mainstyle.css" rel="stylesheet" type="text/css" />
<link href="css/and.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="js/herouter.js"></script>
<script type="text/javascript" src="js/jquery.js"></script>
<script type="text/javascript" src="js/jquery.vegas.min.js"></script>

<style type="text/css">
<!--
body{ color:#676767;}
*{-moz-transition: all 0.3s ease-in;-webkit-transition: all 0.3s ease-in;-o-transition: all 0.3s ease-in;transition: all 0.3s ease-in;}
*:hover{-moz-transition: all 0.3s ease-in;-webkit-transition: all 0.3s ease-in;-o-transition: all 0.3s ease-in;transition: all 0.3s ease-in;}
.int { height:410px; width:670px; overflow:hidden; background:#fcfcfa url(images/fullbg.gif) repeat-x; padding:5px 15px;}
.int h1{ font-size:18px; line-height:50px; border-bottom:1px #d3d3d3 solid;}
.mainsetting{ padding:30px; }
.mainsetting h2{ font-size:14px; line-height:38px;}
.c1{ height:48px;}
.c1 b{ display:block; height:48px; cursor:pointer; width:190px; font-weight:normal; float:left; background:#77d1df; font-size:18px; color:#fff; text-align:center; line-height:48px;}
.c1 a{ background:url(images/help.png) no-repeat center center; display:block; float:left; height:48px; width:30px;}
.c1 b:hover{ background:#56bbcb;}
.c1 span{  font-size:12px; color:#919191; padding:0 0 0 15px; float:left; font-weight:normal; line-height:48px;}
.c1 .err{ background:#e71b1b;}
.c1 .err:hover{ background:#da1515;}
.mainsetting h3{ font-size:24px; font-weight:normal; line-height:50px; color:#29b8e4;}
.c2{ border-top:1px #e6e6e6 solid; margin-top:80px; line-height:30px;}
.c3{ border-top:1px #e6e6e6 solid; margin-top:80px; line-height:30px;}
.c4{ border-top:1px #e6e6e6 solid; line-height:30px; text-align:right; position:relative; bottom:120px}

.cfg{ padding:15px 0; height:330px;}
.cfgleft{ float:left; width:185px; height:330px; background:url(images/cfgleftbg.png) repeat-y;}
.cfgleft ul{}
.cfgleft li{ cursor:pointer; height:40px; line-height:40px; font-size:14px; color:#666; border-left:4px #dfdfdf solid ; padding-left:12px;}
.cfgleft li:hover{ border-left:4px #1e9ec5 solid;  font-weight:bold;}
.cfgleft li.on{ border-left:4px #1e9ec5 solid; color:#1e9ec5; font-weight:bold; font-size:16px; background:#fff url(images/arrow1.png) no-repeat 170px center;}
.cfgright{height:330px;}
.cfgright td{ height:36px; font-size:14px;}
.cfgright td span{ padding-left:100px; font-size:14px;}
.cfgright .spheight{padding-left:100px; height:60px; }
.cfgright td label{ font-size:14px; line-height:60px;}
.radiobox{ margin-top:-2px;}
.inp1 {height:26px; border:1px #d2d2d2 solid; width:160px;font-family:"微软雅黑"; color:#666; padding:0 10px;}
.subpart{ height:48px; padding-left:160px; padding-top:60px;}
.subpart ul{}
.subpart li{ float:left; padding-right:10px;}
.subpart b{ display:block; height:48px; cursor:pointer; width:170px; font-weight:normal; float:left; background:#77d1df; font-size:18px; color:#fff; text-align:center; line-height:48px;}
.subpart b:hover{ background:#56bbcb;}
.subpart .cancel{ width:110px; background:#99e4f0; }

.state{ padding:15px 0; color:#757575;}
.state1{ float:left; width:315px;; padding:10px ;}
.state2{ float:right; width:315px;  padding:10px ;}

.entstate{padding-top:50px;padding-left:230px;}
.entstate h3{font-size:16px; color:#3aaec1; line-height:30px;}
.entstate ul{}
.entstate li{line-height:30px; font-size:14px; height:30px;}
.entstate li b{width:100px; display:block; float:left;}
.entstate li span{display:block; float:left;}
.entstate li a,.intstate li a:visited{ display:block; height:30px; padding:0 12px; background:#77d1df; float:left; color:#fff; margin-left:10px; }
.entstate li a:hover,.intstate li a:active{ text-decoration:none; background:#56bbcb;}

.intstate .intok{float:right;}
.box{ width:500px; height:40px; margin:auto auto; }      
.r1{ height:1px; overflow:hidden; margin:0 5px; background:#56bbcb; }     
.r2{ height:1px; overflow:hidden; margin:0 3px; border-left:2px solid #56bbcb; border-right:2px solid #56bbcb; }      
.r3{ height:1px; overflow:hidden; margin:0 2px; border-left:1px solid #56bbcb; border-right:1px solid #56bbcb; }      
.r4{ height:2px; margin:0 1px; overflow:hidden; border-left:1px solid #56bbcb; border-right:1px solid #56bbcb; }       
.content{ height:30px; border-left:1px solid #56bbcb; border-right:1px solid #56bbcb; padding:7px 10px; } 
.content h2{ color:#56bbcb; font-family:"微软雅黑"; font-size:16px;}
.Txttype {
font-size:20px;
line-height:50px; 
color: #000000;
}
.newHiType{
font-size:16px;
line-height:50px; 
color: #cccccc;

}
-->
</style>


<script type="text/javascript">
var http_request = false;
var rst_buf = new Array();
var quicksetupflag;
var pppoeretflag;

var mode = '<% getCfgGeneral(1, "wanConnectionMode"); %>';
var PingStatus = "<% getPingStatus(); %>";

function isHiddenField(ishidden,hid_id)
{
	if(ishidden == 1){
		document.getElementById(hid_id).style.display = "none";
	}else{
		document.getElementById(hid_id).style.display = "block";
	}
} 
function parseResponse(rtBuf) {

/*response result:
			  cablePlug: pass
			  wanIp: pass
			  siteAccess: pass
			  PPPOEStatus:pass (pass:fail:timeout)
*/
	var wan_ok;
	$("#wait").css("display","none");
	rst_buf=rtBuf.split("\n");
	//WAN SIDE
	if( (rst_buf[0].indexOf("pass") == -1) || (rst_buf[1].indexOf("pass") == -1) ||(rst_buf[2].indexOf("pass") == -1)) {
		//alert("互联网不可达，请检查配置");
		if(rst_buf[0].indexOf("pass") == -1)
		{
		
		if(quicksetupflag=="1")
		    $("h2").text("网线未插,此设置将会在您插上网线后生效,继续设置请点击<跳过>按钮");
           else 
		    $("h2").text("网线未插,此设置将会在您插上网线后生效");
		}
		else if((mode == "PPPOE") && (rst_buf[3].indexOf("fail") != -1))
		{
		    $("h2").text("PPPOE账户或者密码错误，请检查后重新输入");
			pppoeretflag="1";
		}
		else if((mode == "PPPOE") && (rst_buf[3].indexOf("timeout") != -1))
		{
		if(quicksetupflag=="1")
		    $("h2").text("设置已生效但PPPOE拨号失败,未发现远程服务器,继续设置请点击<跳过>按钮");
		else 
		    $("h2").text("设置已生效但PPPOE拨号失败,未发现远程服务器,请检查宽带口的网线是否连接到外网");
		}
		else if((rst_buf[1].indexOf("pass") !=-1)&&(rst_buf[2].indexOf("pass") == -1) )
		{
		   
		if(quicksetupflag=="1")
		    $("h2").text("已获取到IP地址,但无法链接到外网,请在系统状态中查看DNS地址是否存在,继续设置请点击<跳过>按钮");
		else 
		    $("h2").text("已获取到IP地址,但无法链接到外网,请在系统状态中查看DNS地址是否存在");
		}
		else if((mode == "PPPOE") ){
		 
		if(quicksetupflag=="1")
		    $("h2").text("PPPOE服务器暂无响应,请点击<跳过>按钮直接进行下一步操作");
		else 
		    $("h2").text("PPPOE服务器暂无响应,请稍后在系统状态中查看是否拨号成功");
		}
		else 
		{
		    $("h2").text("互联网不可达,请检查配置及网线连接");
		}

		$("div.c2").css("display", "none");
		$("div.c3").css("display", "none");
		$("ul").css("display","block");
	}else{
		$("h2").text("互联网畅通，可以上网冲浪");
		if(quicksetupflag=="1")
		{
			document.getElementById("lineinfo").innerHTML = "<p>3秒后自动跳到下一个配置页面 &gt;&gt;<span id=\"dd\">3</span></p>";
		}
		else
		{
			document.getElementById("lineinfo").innerHTML = "<p>3秒后自动关闭当前页面 &gt;&gt;<span id=\"de\">3</span></p>";
		}
		window.setInterval("run2();", 1000);
	}
	if(quicksetupflag=="1")
	{
		isHiddenField(0,"renetwizard");
		isHiddenField(1,"renetconfig");
		isHiddenField(0,"setquickwifi");
		isHiddenField(1,"closewebpage");
	}
	else
	{
		isHiddenField(1,"renetwizard");
		isHiddenField(0,"renetconfig");
		isHiddenField(1,"setquickwifi");
		isHiddenField(0,"closewebpage");
	}
}

function ReQueryNetStateResult()
{
	http_request.onreadystatechange = NetStateResult;
	http_request.open('POST', "/goform/netState", true);
	http_request.send("netState=true");
}

function NetStateResult() {
	if (http_request.readyState == 4) {
	//alert(http_request.status);
	if (http_request.status == 200) {
			parseResponse(http_request.responseText);
		}
		else
		{
			setTimeout(ReQueryNetStateResult,2000);
		}
	}
}

function requestNetState() {
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
			alert('诊断初始化失败');
			return false;
		}
		
		http_request.onreadystatechange = NetStateResult;
		http_request.open('POST', "/goform/netState", true);
		http_request.send("netState=true");
}

function reboot_internet() {
      $.ajax({
      	type: "get",
        url: "/goform/doInitInternet",     
        data: $("#internet_status").serialize(),    
        success: function(data){
        },
        error: function(data) {
        }
      })
}

function GetQueryString(name)
{
	var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)", "i");
	var r = window.location.search.substr(1).match(reg);
	if (r != null) return unescape(r[2]); return null;
}

function run(){
    var s = document.getElementById("wait");
    if(s.innerHTML == 0){
        return false;
    }
    s.innerHTML = s.innerHTML * 1 - 1;
}
function run2(){
	if(quicksetupflag=="1")
	{
		var s = document.getElementById("dd");
		if(s.innerHTML == 0){
			location.href="/herouter/QuickSetwifi.asp";
			return false;
		}
		s.innerHTML = s.innerHTML * 1 - 1;
	}
	else
	{
		var s = document.getElementById("de");
		if(s.innerHTML == 0){
			parent.TINY.box.hide();
			return false;
		}
		s.innerHTML = s.innerHTML * 1 - 1;
	}
}

function wizardreconfig(){
	window.location.href="QuickSetUp.asp?netmode="+pppoeretflag;
}

function renetconfig(){
	window.location.href="intconfig.asp";
}

function jump(){
	window.location.href="QuickSetwifi.asp";
}

$(document).ready(function(){

	quicksetupflag = GetQueryString("quicksetup");
	reboot_internet();
	document.getElementById("msg").innerHTML = "正在设置路由器，请稍候...<span id=\"wait\">32</span>秒";
	window.setInterval("run();", 1000);	
	setTimeout(requestNetState, 30000);

	if (mode == "DHCP"){
		document.getElementById("connectmode").innerHTML = "<b>上网方式：</b><span>自动获取（DHCP）</span>"; 
		$("li#pppoename").css("visibility", "hidden");
		$("li#pppoepwd").css("visibility", "hidden");
	}else if (mode == "STATIC"){
		$("li#pppoename").css("visibility", "hidden");
		$("li#pppoepwd").css("visibility", "hidden"); 
	}else if(mode == "PPPOE"){
		document.getElementById("connectmode").innerHTML = "<b>上网方式：</b><span>宽带拨号（PPPoE）</span>"; 
		$("li#pppoename").css("visibility", "visible");
		$("li#pppoepwd").css("visibility", "visible");
	}
	if(quicksetupflag=="1"){
  document.getElementById("Toptitle").innerHTML='<span class="Txttype">(第一步)互联网设置<em>--></em></span><span class="newHiType">(第二步)无线设置—>(第三步)完成</span>';
}

});

</script>

</head>

<body>
<form method="post" name="internet_status" id="internet_status" action="/goform/doInitInternet">
<div class="int">
	<h1 id="Toptitle" align="center">互联网设置</h1>
    <div class="cfg">
			<div class="box">   
				<div class="top">
					<div class="r1"></div>
					<div class="r2"></div>
					<div class="r3"></div>
					<div class="r4"></div>
				</div>
				<div class="content">
						<table style="margin:auto;">
        			<tr>
								<td><img src="./images/check3_07.png"/></td>
								<td>&nbsp;&nbsp;</td>
     						<td>
     							<h2 id="msg"></h2>
     						</td>
        			</tr> 
						</table>					
				</div>
				<div class="bottom">
					<div class="r4"></div>
					<div class="r3"></div>
					<div class="r2"></div>
					<div class="r1"></div>
				</div>
			</div>    	  	
        <div class="cfgright">    	 	
            <div class="state">
					  	<div class="entstate">
				        <ul>
				          <li id="connectmode"><b>上网方式：</b><span>
				            <% getCfgGeneral(1, "wanConnectionMode"); %>
				          </span></li>				        	
				          <li id="pppoename"><b>PPPOE账户：</b><span>
				            <% getCfgGeneral(1, "wan_pppoe_user"); %>
				          </span></li>
				          <li id="pppoepwd"><b>PPPOE密码：</b><span>
				            <% getCfgGeneral(1, "wan_pppoe_pass"); %>
				          </span></li>
				        </ul>
							</div>				                     	
						</div>            	
            <div class="subpart">
                <ul style="display:none">
					<li style="display:none;" id="renetwizard" onClick="wizardreconfig()"><b>重新设置</b></li>
					<li style="display:;" id="renetconfig" onclick="renetconfig();"><b>重新设置</b></li>
					<li style="display:none;" id="setquickwifi" onclick="jump();"><b>跳过</b></li>
					<li style="display:;" id="closewebpage" class="cancel" onclick="javascript:parent.TINY.box.hide();"><b>关闭</b></li>
                </ul>
            </div>
        </div>
      <div id="lineinfo" class="c4"></div>
    </div>    
</div>
</form>
</body>
</html>
