<!DOCTYPE html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<meta name="viewport" content="width=device-width,initial-scale=1.0,minimum-scale=1.0,maximum-scale=1.0,user-scalable=no" />
<title>和路由-中国移动</title>
<link href="css/mainstyle.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="js/jquery.js"></script>
<script type="text/javascript" src="js/jquery.vegas.min.js"></script>
<script type="text/javascript" src="js/tinybox.js"></script>
<script type="text/javascript" src="js/base64.js"></script>
<script type="text/javascript" src="js/herouter.js"></script>
<link href="css/jquery.vegas.css" rel="stylesheet" type="text/css" />
<link href="css/and.css" rel="stylesheet" type="text/css" />
<script type="text/javascript">
var hh=$(window).height()-20;
var ww=$(window).width()-20;
var xmlhttp;

$(function() {
  $.vegas({
    src:'images/bodybg.jpg'
  });
});

function create_base_auth(user, password) {
  var tok = user + ':' + password;
  
  var obj=new Base64();
  var hash=obj.Encode64(tok);
  return "Basic " + hash;
} 


$(document).keydown(function(event){ 
    if(event.keyCode == 13){ 
        sendAuth();
    }
});

function callback(){  
  if (xmlhttp.readyState== 4) {
    if (xmlhttp.status == 200){  
    	
    	var ResText = xmlhttp.responseText;
    	var needLogin = isLogin(ResText);
      if(0 == needLogin){
          setTimeout("javascript:location.href='/herouter/mainformobile.asp'", 1);
      }
      else if(1 == needLogin){    
      	  alert("输入错误,请重新输入.");
          window.location.reload();
      }
    }  
  }  
}

function sendAuthNew(){
	
			var maccept = "image/gif, image/ico, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/xaml+xml, application/x-ms-xbap, application/x-ms-application, */*"
					
			with ( document.forms[0] ) {
  	 		var pwd = mpassword.value;
			}
			var auth = create_base_auth('admin',pwd);
			var needLogin = -1;
				
	    $.ajax({
      	type: "GET",
        url: "/",     
        //dataType: 'jsonp', 
        //jsonp: 'jsoncallback',
    		timeout: 5000,
        async: false,
        beforeSend: function (xhr) {
        xhr.setRequestHeader("Accept", maccept);
        xhr.setRequestHeader("Authorization", auth);
    		}, 
		    success: function (xmlDoc, textStatus, xhr) {
		    		var oXmlUPnP = xmlDoc;

		        var ResText = xhr.responseText;
		        needLogin = isLogin(ResText);
		        
		        //alert(needLogin);
			      if(0 == needLogin){
				    	setTimeout("javascript:location.href='/herouter/mainformobile.asp'", 1);
				    	return true;
				    }
		    }
      })
      
      if ((0 != needLogin))
      {
      	alert("输入错误,请重新输入.");
				window.location.reload();
				return false;
      }
}

function sendAuth() {
	
	var maccept = "image/gif, image/ico, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/xaml+xml, application/x-ms-xbap, application/x-ms-application, */*"

  with ( document.forms[0] ) {
  	 var pwd = mpassword.value;
	}	
	
 	var auth = create_base_auth('admin',pwd);
  xmlhttp = new XMLHttpRequest();
  xmlhttp.onreadystatechange = callback;
  xmlhttp.open('GET','/'); 
  xmlhttp.setRequestHeader("Authorization", auth);
  xmlhttp.setRequestHeader("Accept", maccept);
  xmlhttp.send(null);
} 

</script>
<style type="text/css">
<!--
.loginbg{  height:110px;  }
.loginpart{ height:225px; padding-left:10px; width:300px; margin:0 auto; background:url(images/mob/logbg.png) no-repeat center center; background-size:320px 205px; -moz-border-radius: 6px;border-radius: 6px;-webkit-border-radius: 6px; }
.loginpart h2{ padding-top:70px; line-height:40px; color:#fff; font-weight:normal; font-size:18px;}
.loginpart .err{ color:#fb0000;}
.loginpart .inparea{ height:36px;}
.loginpart .inparea input{ border:none; height:36px; width:180px; float:left; padding:0 5px; font-size:14px; line-height:36px; color:#666;}
.loginpart .logbtn{ padding:10px 0 0 90px; height:40px;}
.loginpart .logbtn b{display:block; height:36px; cursor:pointer; width:100px; font-weight:normal; float:left; background:#29b8e4; font-size:18px; color:#fff; text-align:center; line-height:36px;-moz-transition: all 0.3s ease-in;-webkit-transition: all 0.3s ease-in;-o-transition: all 0.3s ease-in;transition: all 0.3s ease-in;}
.loginpart .logbtn b:hover{ background:#17a3ce;-moz-transition: all 0.3s ease-in;-webkit-transition: all 0.3s ease-in;-o-transition: all 0.3s ease-in;transition: all 0.3s ease-in;}
-->
</style>

</head>

<body>
<form>
<div class="loginbg"></div>
<div class="loginpart">
	<h2 class="err">请输入和路由管理员密码</h2>
	<div class="inparea"><input name="mpassword" type="password" />
	<span></span>
	</div>
		<font color="#FFECEC"><h4>(初始密码:123456)</h4></font>
	<div class="logbtn"><b onclick="javascript:sendAuthNew()">登录</b></div>
</div>
</form>
</body>
</html>
