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
.wifihide{ }
.hidenet{ float:right;background:#29bae6; padding:2px; -moz-border-radius: 5px;border-radius: 5px;-webkit-border-radius: 5px; height:32px; overflow:hidden; }
.checkinp{height:26px; width:26px; padding:0; border:none; background:#f5f5f5;}
.hidenet span{ font-size:14px; color:#fff; line-height:32px; padding-right:10px;}
.submit{}
.submit strong{height:45px; display:block; background:#29bae6; color:#fff; line-height:45px; text-align:center; font-size:18px; cursor:pointer; }
-->
</style>
<script type="text/javascript">
function $$$$$(_sId){
 return document.getElementById(_sId);
 }
function hide(_sId){
	$$$$$(_sId).style.display = $$$$$(_sId).style.display == "none" ? "" : "none";
 }
function pick(v,targetid,abc) {
	document.getElementById(targetid).value=v;
	hide(abc);
}
var switchcode=0;
$(document).ready(function(){
	$("#switch").click(function(){
		if(switchcode==0){
			 $("#state").addClass("switchoff");
			 switchcode=1;
			}else{
			$("#state").removeClass("switchoff");
			switchcode=0;
			}
	})
	$('#list').change(function(){ 
	$("#listselect").val($("#list").find("option:selected").text());
	});
});



</script>
</head>

<body>
<div class="setting">
	<h1><b>修改密码</b><span onClick="javascript:parent.TINY.box.hide();"></span></h1>
    <h2>*修改路由管理界面登入密码</h2>
    <div class="conf">
    	
        <ul>
       	  <li><input class="inp1" name="" type="text" placeholder="原密码"></li>
          <li><input class="inp1" name="" type="password" placeholder="新密码"></li>
          <li><input class="inp1" name="" type="password" placeholder="新密码确认"></li>
          <li class="submit"><strong>保   存</strong></li>
        </ul>
    
    </div>
</div>
</body>
</html>
