<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Flot Examples: Real-time updates</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link href="../examples.css" rel="stylesheet" type="text/css">
<!--[if lte IE 8]><script language="javascript" type="text/javascript" src="../excanvas.min.js"></script><![endif]-->
<script language="javascript" type="text/javascript" src="../jquery.js"></script>
<script language="javascript" type="text/javascript" src="../jquery.flot.js"></script>
<script type="text/javascript">
var totalPoints = 100;
var updateInterval = 2000;
var bytecount1 = new Array();
var bytecount2 = new Array();
var bytecount3 = new Array();
var bytecount4 = new Array();
var time = new Array();
var bssid1_rate = new Array();
var bssid2_rate = new Array();
var bssid3_rate = new Array();
var bssid4_rate = new Array();
var http_request = false;
var ymax;

function makeRequest(url, content) 
{
	http_request = false;
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
		http_request = new XMLHttpRequest();
		if (http_request.overrideMimeType) 
			http_request.overrideMimeType('text/xml');
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
		alert('Giving up : Cannot create an XMLHTTP instance');
		return false;
	}
	http_request.onreadystatechange = dataHandler;
	http_request.open('POST', url, true);
	http_request.send(content);
}

function collectByteCount(str)
{
	var data = str.split(";");
	if (bytecount1.length == totalPoints+1)
		bytecount1 = bytecount1.slice(1);
	bytecount1.push(data[0]);
	if (bytecount2.length == totalPoints+1)
		bytecount2 = bytecount2.slice(1);
	bytecount2.push(data[1]);
	if (bytecount3.length == totalPoints+1)
		bytecount3 = bytecount3.slice(1);
	bytecount3.push(data[2]);
	if (bytecount4.length == totalPoints+1)
		bytecount4 = bytecount4.slice(1);
	bytecount4.push(data[3]);
	if (time.length == totalPoints+1)
		time = time.slice(1);
	time.push(data[4]);
	//alert(bytecount1);
	//alert(bytecount2);
	//alert(bytecount3);
	//alert(bytecount4);
	//alert(time);
}

function updateDataRate()
{
	var input;
	var output;
	var factor = document.getElementById("factor").value;
	var len = bytecount1.length;
	
	if (len < 2)
		return;

	if (bssid1_rate.length == totalPoints)
		bssid1_rate = bssid1_rate.slice(1);
	if (bssid2_rate.length == totalPoints)
		bssid2_rate = bssid2_rate.slice(1);
	if (bssid3_rate.length == totalPoints)
		bssid3_rate = bssid3_rate.slice(1);
	if (bssid4_rate.length == totalPoints)
		bssid4_rate = bssid4_rate.slice(1);

	input = (bytecount1[len-1]-bytecount1[len-1-1])/(time[len-1]-time[len-1-1]);
	if (len > 2)
		output = (input*factor)+(bssid1_rate[bssid1_rate.length-1]*(1-factor));
	else
		output = input;
	bssid1_rate.push(output);

	input = (bytecount2[len-1]-bytecount2[len-1-1])/(time[len-1]-time[len-1-1]);
	if (len > 2)
		output = (input*factor)+(bssid2_rate[bssid2_rate.length-1]*(1-factor));
	else
		output = input;
	bssid2_rate.push(output);

	input = (bytecount3[len-1]-bytecount3[len-1-1])/(time[len-1]-time[len-1-1]);
	if (len > 2)
		output = (input*factor)+(bssid3_rate[bssid3_rate.length-1]*(1-factor));
	else
		output = input;
	bssid3_rate.push(output);

	input = (bytecount4[len-1]-bytecount4[len-1-1])/(time[len-1]-time[len-1-1]);
	if (len > 2)
		output = (input*factor)+(bssid4_rate[bssid4_rate.length-1]*(1-factor));
	else
		output = input;
	bssid4_rate.push(output);
	//alert(factor);
	//alert(bssid1_rate);
	//alert(bssid2_rate);
	//alert(bssid3_rate);
	//alert(bssid4_rate);
}

function dataHandler() {
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			collectByteCount(http_request.responseText);
			updateDataRate();
		} else {
			alert('There was a problem with the request.');
		}
	}
}

function queryData() {
	makeRequest("/goform/getVoW", "rai0");
}

$(function() 
{
	// We use an inline data source in the example, usually data would
	// be fetched from a server

	function getData(field) 
	{
		var res1 = new Array();
		var res2 = new Array();
		var res3 = new Array();
		var res4 = new Array();

		for (var i = 0; i < totalPoints; ++i) 
		{
			res1.push([i, bssid1_rate[i]]);
			res2.push([i, bssid2_rate[i]]);
			res3.push([i, bssid3_rate[i]]);
			res4.push([i, bssid4_rate[i]]);
		}
		//alert("res1:"+res1);
		//alert("res2:"+res2);
		//alert("res3:"+res3);
		//alert("res4:"+res4);

		if (field == 4)
			return res4;
		else if (field == 3)
			return res3;
		else if (field == 2)
			return res2;
		else if (field == 1)
			return res1;
	}

	// Set up the control widget

	$("#updateInterval").val(updateInterval).change(function () {
		var v = $(this).val();
		if (v && !isNaN(+v)) {
		updateInterval = +v;
		if (updateInterval < 1) {
		updateInterval = 1;
		} else if (updateInterval > 2000) {
		updateInterval = 2000;
		}
		$(this).val("" + updateInterval);
		}
	});

	var plot1 = $.plot("#placeholder1", [getData(1),getData(2),getData(3),getData(4)], {
		series: {
			shadowSize: 0	// Drawing is faster without shadows
		},
		yaxis: {
			min: 0,
			max: 60
		},
		xaxis: {
			show: false
		}
	});

	function update() {
		var y_axis = ymax;

		//alert("update");
		queryData();
		ymax = document.getElementById("ymax").value;
		if (y_axis == ymax)
		{
			plot1.setData([getData(1),getData(2),getData(3),getData(4)]);
		}
		else
		{
			$.plot("#placeholder1", [getData(1),getData(2),getData(3),getData(4)], {
				series: {
					shadowSize: 0   // Drawing is faster without shadows
				},
				yaxis: {
					min: 0,
					max: ymax
				},
				xaxis: {
					show: false
				}
			});
		}

		// Since the axes don't change, we don't need to call plot.setupGrid()

		plot1.draw();
		setTimeout(update, updateInterval);
	}

	update();
});

function StylDispOn()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-cell";
	}
}

function switchDebugMode()
{
	if (document.forms[0].debug.checked == true)
	{
		document.getElementById("div_factor").style.visibility = "visible";
		document.getElementById("div_factor").style.display = StylDispOn();
	}
	else
	{
		document.getElementById("div_factor").style.visibility = "hidden";
		document.getElementById("div_factor").style.display = "none";
	}
}
</script>
</head>
<body onload="switchDebugMode();">
<h2>Voice/Video over Wireless</h2>
<table width="540" border="1" cellspacing="1" cellpadding="3" bordercolor="#9BABBD">
<tr>
<td>
<div class="demo-container">
<div id="placeholder1" class="demo-placeholder"></div>
</div>
</td>
</tr>
</table>

<table border="0" cellpadding="2" cellspacing="1" width="540">
  <tbody>
  <tr>
    <td>
      <form>
    	<input type=checkbox name="debug" onClick="switchDebugMode();">debug mode
      </form>
    </td>
    <td id="div_factor">
      expinential moving average factor:
      <select id="factor" name="factor" size="1">
	<option value="1">1</option>
	<option value="0.5">1/2</option>
	<option value="0.25" selected>1/4</option>
	<option value="0.125">1/8</option>
	<option value="0.0625">1/16</option>
      </select>
    </td>
    <td>
      y-axis upper limit:
      <select id="ymax" name="ymax" size="1">
	<option value="100">100</option>
	<option value="80">80</option>
	<option value="60" selected>60</option>
	<option value="40">40</option>
	<option value="20">20</option>
      </select>
    </td>
  </tr>
  </tbody>
</table>
<hr />
<br />
</body>
</html>
