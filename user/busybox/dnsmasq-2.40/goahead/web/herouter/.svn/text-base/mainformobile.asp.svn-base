<!DOCTYPE html>
<head>
<meta name="KEYWords" Content="mainPageFlag">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<meta name="viewport" content="width=device-width,initial-scale=1.0,minimum-scale=1.0,maximum-scale=1.0,user-scalable=no" />
<title>和路由-中国移动</title>
<link href="css/mainstyle.css" rel="stylesheet" type="text/css">
<script type="text/javascript" src="js/jquery.js"></script>
<script type="text/javascript" src="js/jquery.vegas.min.js"></script>
<script type="text/javascript" src="js/tinybox.js"></script>
<link href="css/jquery.vegas.css" rel="stylesheet" type="text/css" />
<script type="text/javascript">

var WIFIAuthMode = "<% getCfgGeneral(1, "AuthMode"); %>";
var wifi_off='<% getCfgZero(1, "WiFiOff"); %>';
var EncrypType='<% getCfgZero(1, "EncrypType"); %>';	
var IEEE8021X  = '<% getCfgZero(1, "IEEE8021X"); %>';
var ConMode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
var WANIP = "<% getWanIp(); %>";
var isNeedWizard = "<% getCfgGeneral(1, "needWizard"); %>";

var hh=$(window).height()-20;
var ww=$(window).width()-20;
//alert(hh);
$(function() {
  $.vegas({
    src:'images/bodybg.jpg'
  });
});

$(document).ready(function(){
	
	//wifi enable
	if(wifi_off == 0){
		  //no auth mode(disable)
		  $("#btn2").addClass("btnok");
		  if(WIFIAuthMode == "OPEN" && EncrypType == "NONE" && IEEE8021X == "0"){ 
			    $("#wifistate").addClass("wifialert");
			  //like wep,wpapsk...  
	    }else{
          $("#wifistate").addClass("wifistate");
	    }   
	}else{
			
			$("#btn2").addClass("btnerr");
			$("#wifistate").addClass("wifierr");
	}

	//internet ico
  if (WANIP == "")	{
  	  
	  $("#btn1").addClass("btnerr");
      $("#intstate").addClass("interr");	 
  }else{
  	  $("#btn1").addClass("btnok");
  	  $("#intstate").addClass("intstate"); 
  }
	if(isNeedWizard == 1)
	{
      TINY.box.show({iframe:'mobintsetting.asp',boxid:'frameless',width:310,height:hh,fixed:false,close:false,maskopacity:40})
	}
	
});
function reload_main()
{
  window.location.href="/herouter/mainformobile.asp";
}
function showConfProcess(){
	setTimeout("TINY.box.show({html:'<div class=waiting3 style=display:;><h2>设置成功，无线网络将断开，请关闭页面并在30秒后重新进入</h2><div class=animation></div></div>',animate:false,close:false,boxid:'',top:5})",3000);
	setTimeout("TINY.box.hide();reload_main();", 20000);
}
</script>
<style type="text/css">
<!--
.mobmain{}
.head { height:60px; background:url(images/mob/headbg.png) repeat-x center top; background-size:100% 60px; min-width:320px;}
.head .logo{ background:url(images/logo_sys.png) no-repeat center center; background-size:180px 30px; float:left; height:60px; width:206px;}
.head b{ float:right; height:50px; margin:5px 0; width:82px; border-left:1px #e1dedb solid; text-align:center; line-height:50px; font-size:14px; font-weight:normal; color:#fff; cursor:pointer;}
.tip{ background:url(images/mob/tipbg.png) repeat-x; height:32px; width:100%; min-width:320px; padding:4px 0; background-size:100% 40px; position:absolute;}
.tipclose{}
.tip a{ display:block; float:left; height:32px; line-height:32px; color:#fff; font-size:14px; text-align:center; width:125px; border-right:1px #dbdfe4 solid; }

.tip span{ display:block; float:left; height:32px; width:145px; padding-left:5px; line-height:16px; font-size:10px; color:#fff;}
.tip b{ display:block; cursor:pointer; float:right; height:32px; width:32px; background:url(images/mob/btnclose.png) no-repeat center center; background-size:20px 20px;}
.mainsetting{ width:320px; margin:0 auto; padding:70px 0; height:180px; background:url(images/mob/mainbg.png) no-repeat center 70px; background-size:310px 150px; }
.c1{ height:38px; padding-top:56px;}
.int{ padding:0 0 0 15px; float:left;}
.int span{ float:left; padding-left:5px;}
.btnok{  height:38px; width:135px;  -moz-border-radius: 19px;border-radius: 19px;-webkit-border-radius: 19px; background:#29bae6; -moz-box-shadow:3px 3px 0px #c5c7c8;-webkit-box-shadow:3px 3px 0px #c5c7c8;box-shadow:3px 3px 0px #c5c7c8;cursor:pointer;}
.btnerr{  height:38px; width:135px;  -moz-border-radius: 19px;border-radius: 19px;-webkit-border-radius: 19px; background:#eb2020; -moz-box-shadow:3px 3px 0px #c5c7c8;-webkit-box-shadow:3px 3px 0px #c5c7c8;box-shadow:3px 3px 0px #c5c7c8;cursor:pointer;}
.btnok span{ font-size:16px; line-height:38px; color:#fff;}
.btnerr span{ font-size:16px; line-height:38px; color:#fff;}
.wifi{padding:0 15px 0 0; float:right;}
.wifi span{ float:right; padding-right:5px;}
.intstate{ margin:3px; height:32px; width:32px; float:left;-moz-border-radius: 14px;border-radius: 14px;-webkit-border-radius: 14px; background:#fff url(images/mob/intstate.png) no-repeat center 2px; background-size:30px auto;}
.wifistate{ margin:3px; height:32px; width:32px; float:right;-moz-border-radius: 14px;border-radius: 14px;-webkit-border-radius: 14px; background:#fff;background:#fff url(images/mob/wifistate.png) no-repeat center 2px; background-size:30px auto; }

.intstate .icon{ height:32px; width:32px; background:url(images/mob/checked.png) no-repeat center 0; background-size:30px auto; display:block; }
.wifierr{ margin:3px; height:32px; width:32px; float:right;-moz-border-radius: 14px;border-radius: 14px;-webkit-border-radius: 14px; background:#fff;background:#fff url(images/mob/wifistate.png) no-repeat center -28px; background-size:30px auto; }
.wifialert{margin:3px; height:32px; width:32px; float:right;-moz-border-radius: 14px;border-radius: 14px;-webkit-border-radius: 14px; background:#fff;background:#fff url(images/mob/wifistate.png) no-repeat center 2px; background-size:30px auto; }
.wifistate .icon{ height:32px; width:32px; background:url(images/mob/checked.png) no-repeat center 0; background-size:30px auto; display:block;}

.interr{margin:3px; height:32px; width:32px; float:left;-moz-border-radius: 14px;border-radius: 14px;-webkit-border-radius: 14px; background:#fff url(images/mob/intstate.png) no-repeat center -28px; background-size:30px auto;}
.interr .icon{height:32px; width:32px; background:url(images/mob/checked.png) no-repeat center -30px; background-size:30px auto; display:block;}
.wifierr .icon{ height:32px; width:32px; background:url(images/mob/checked.png) no-repeat center -30px; background-size:30px auto; display:block;}
.wifialert .icon{ height:32px; width:32px; background:url(images/mob/checked.png) no-repeat center -60px; background-size:30px auto; display:block;}

.linked{ padding-top:50px;}
.linkedbtn{ width:190px; height:38px;-moz-border-radius: 19px;border-radius: 19px;-webkit-border-radius: 19px; background:#29bae6 ; -moz-box-shadow:3px 3px 0px #c5c7c8;-webkit-box-shadow:3px 3px 0px #c5c7c8;box-shadow:3px 3px 0px #c5c7c8; margin:0 auto; font-size:16px; line-height:38px; text-align:center; color:#fff; cursor:pointer;}
.linkedbtn b{ font-size:18px;}
.footer{position:absolute; bottom:0px; color:#fff; font-size:12px; line-height:36px;-ms-filter: "progid:DXImageTransform.Microsoft.Alpha(Opacity=75)";   filter:alpha(opacity=75);  opacity: .75; text-align:center; width:100%; }
.footer a{ color:#fff; padding:0 20px;}

.tbox {position:absolute; display:none; padding:0; z-index:999999;}
.tinner {padding:0px; -moz-border-radius:5px; border-radius:5px; overflow:hidden; background:#ccc;}
.tmask {position:absolute; display:none; top:0px; left:0px; height:100%; width:100%; background:#000; z-index:99999;opacity: 0.7;-moz-transition: all 0.3s ease-in;-webkit-transition: all 0.3s ease-in;-o-transition: all 0.3s ease-in;transition: all 0.3s ease-in;}
.tclose {position:absolute; top:0px;right:0px; width:60px; height:60px; cursor:pointer; background:url(../images/close.png) no-repeat}
.tclose:hover {background-position:0 -60px}

#error {background:#ff6969; color:#fff; text-shadow:1px 1px #cf5454; border-right:1px solid #000; border-bottom:1px solid #000; padding:0}
#error .tcontent {padding:10px 14px 11px; border:1px solid #ffb8b8; -moz-border-radius:5px; border-radius:5px}
#success {background:#2ea125; color:#fff; text-shadow:1px 1px #1b6116; border-right:1px solid #000; border-bottom:1px solid #000; padding:10; -moz-border-radius:0; border-radius:0}
#bluemask {background:#4195aa}
#frameless {padding:0}
#frameless .tclose {left:700px}

-->
</style>

</head>

<body>
<div class="mobmain">
	<div class="head">
		<div class="logo"></div>
	</div>
	<!--div class="tip">
		<a href="#">下载手机客户端</a><span>随时随地设置您网络中的设备，真正远程管理的路由！</span><b></b></p>
	</div-->
	<div class="mainsetting">
   	  <div class="c1">
            <div class="int">
                <div id="btn1" onClick="TINY.box.show({iframe:'mobintsetting.asp',boxid:'frameless',width:310,height:hh,fixed:false,close:false,maskopacity:40})">
                    <div id="intstate"><b class="icon"></b></div>
                    <span>互联网</span>
                </div>
            </div>
            <div class="wifi">
                <div id="btn2" onClick="TINY.box.show({iframe:'mobwifisetting.asp',boxid:'frameless',width:310,height:hh,fixed:false,close:false,maskopacity:40})">
                    <div id="wifistate"><b class="icon"></b></div>
                    <span>无线</span>
                </div>
            </div>
        </div>
      
	</div>
    
</div>
<div class="footer"><a href="http://www.heluyou.com/herouter/main.asp">电脑版网页</a> | <a href="http://www.andluyou.com/">官方网站</a></div>
</body>
</html>
