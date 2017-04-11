<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Flot Examples: Real-time updates</title>
<link href="../examples.css" rel="stylesheet" type="text/css">
<!--[if lte IE 8]><script language="javascript" type="text/javascript" src="../excanvas.min.js"></script><![endif]-->
<script language="javascript" type="text/javascript" src="../jquery.js"></script>
<script language="javascript" type="text/javascript" src="../jquery.flot.js"></script>
<script type="text/javascript">
var totalPoints = 10;
var updateInterval = 2000;
var cpu_idle = new Array();
var mem_free = new Array();
var wait_io = new Array();
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

function dataHandler() {
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			var str = http_request.responseText.split("\n");
			var factor = 0.25;
			var output;
			for (var i=0; i<str.length; i++)
			{
				var loading = str[i].split(";");
				if (cpu_idle.length == totalPoints)
					cpu_idle = cpu_idle.slice(1);
				if (cpu_idle.length > 0)
					output = (loading[0]*factor)+(cpu_idle[cpu_idle.length-1]*(1-factor));
				else
					output = loading[0];
				cpu_idle.push(output);
				if (mem_free.length == totalPoints)
					mem_free = mem_free.slice(1);
				if (mem_free.length > 0)
					output = (loading[1]*factor)+(mem_free[mem_free.length-1]*(1-factor));
				else
					output = loading[1];
				mem_free.push(output);
				if (wait_io.length == totalPoints)
					wait_io = wait_io.slice(1);
				if (wait_io.length > 0)
					output = (loading[2]*factor)+(wait_io[wait_io.length-1]*(1-factor));
				else
					output = loading[2];
				wait_io.push(output);
			}
			//alert(cpu_idle);
			//alert(mem_free);
			//alert(wait_io);
		} else {
			alert('There was a problem with the request.');
		}
	}
}

function queryData() {
	makeRequest("/goform/getSystemLoading", "1");
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
		for (var i = 0; i < totalPoints ; ++i) 
		{
			res1.push([i, cpu_idle[i]]);
			res2.push([i, mem_free[i]]);
			res3.push([i, wait_io[i]]);
		}
		//alert("res1: "+res1+" cpu: "+cpu_idle);
		//alert("res2: "+res2+" mem: "+mem_free);
		//alert("res3: "+res3+" io: "+wait_io);

		if (field == 3)
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

	var plot = $.plot("#placeholder", [getData(1),getData(2),getData(3)], {
		series: {
			shadowSize: 0	// Drawing is faster without shadows
		},
		yaxis: {
			min: 0,
			max: 100
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
		if (ymax == y_axis)
		{
			plot.setData([getData(1),getData(2),getData(3)]);
		}
		else
		{
			plot = $.plot("#placeholder", [getData(1),getData(2),getData(3)], {
				series: {
					shadowSize: 0	// Drawing is faster without shadows
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

		plot.draw();
		setTimeout(update, updateInterval);
	}

	update();

	// Add the Flot version string to the footer

	$("#footer").prepend("Flot " + $.plot.version + " &ndash; ");
});

</script>
</head>
<body>
<div id="header">
</div>
<div id="content">
<div class="demo-container">
<div id="placeholder" class="demo-placeholder"></div>
</div>
</div>
      y-axis upper limit:
      <select id="ymax" name="ymax" size="1">
	<option value="100">100</option>
	<option value="80">80</option>
	<option value="60" selected>60</option>
	<option value="40">40</option>
	<option value="20">20</option>
      </select>
</body>
</html>
