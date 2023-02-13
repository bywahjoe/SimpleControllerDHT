<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
	<title>Test View Data</title>
</head>
<body>

<?php
require 'config.php';

$conn = new mysqli($servername, $userID, $pass, $database);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 
$dev="WAHYUESP1";
$sql = "SELECT * FROM myesp WHERE DEVICEID='".$dev."'";
$s1=$s2=$s3=$s4=0; 

if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $t1 = $row["DEVICEID"];
        $t2 = $row["DHT1"];
        $t3 = $row["DHT2"];
        $t4 = $row["HUMID1"];
        $t5 = $row["HUMID2"];
        $md=$row["MODE"];
        
        $s1=$row["vminDHT1"];
        $s2=$row["vmaxDHT1"];
        $s3=$row["vminDHT2"];
        $s4=$row["vmaxDHT2"];
      
        echo ' 
            <b>DEVICEID -> ' . $t1 . '</b> <br>
            <b>LAST DHT1-> ' . $t2 . ' °C</b> <br>
            <b>LAST DHT2-> ' . $t3 . ' °C</b> <br><br>
            <b>LAST HUMID1-> ' . $t4 . ' </b> <br>
            <b>LAST HUMID2-> ' . $t5 . ' </b> <br><br>
              ';
    }       
    $result->free();
}

$conn->close();

echo ' 
    <form action="/setThreshold.php" method="post">
    <label for="mode">Select Mode:</label>
    <select name="mode" id="mode">';
if($md=="A"){
    echo '
    <option value="A" selected="selected">Mode A | X > High Threshold is ON</option>
    <option value="B">Mode B | X > High Threshold is OFF</option>';
}else{
        echo '
    <option value="A">Mode A | X > High Threshold is ON</option>
    <option value="B" selected="selected">Mode B | X > High Threshold is OFF</option>';
    
    
}
echo '
<br><br>
  </select><br><br>
  <label for="thr">Set Threshold (between -50 and 50): </label>
  <br>
  <p>-[S1 Min] New Threshold: <span id="slid1"></span></p>
  <input type="range" id="DHT1Min" name="min1" min="-50" max="50" value='.$s1.'>
  <p>-[S1 Max] New Threshold: <span id="slid2"></span></p>
  <input type="range" id="DHT1Max" name="max1" min="-50" max="50" value='.$s2.'>
  <p>-[S2 Min] New Threshold: <span id="slid3"></span></p>
  <input type="range" id="DHT2Min" name="min2" min="-50" max="50" value='.$s3.'>
  <p>-[S2 Max] New Threshold: <span id="slid4"></span></p>
  <input type="range" id="DHT2Max" name="max2" min="-50" max="50" value='.$s4.'>
  <input type="hidden" name="dev" value='.$t1.'>
  <input type="submit" value="Set Config">
</form>';
echo '<script>
var slider1 = document.getElementById("DHT1Min");
var slider2 = document.getElementById("DHT1Max");
var slider3 = document.getElementById("DHT2Min");
var slider4 = document.getElementById("DHT2Max");

var output1 = document.getElementById("slid1");
output1.innerHTML = slider1.value;
var output2 = document.getElementById("slid2");
output2.innerHTML = slider2.value;
var output3 = document.getElementById("slid3");
output3.innerHTML = slider3.value;
var output4 = document.getElementById("slid4");
output4.innerHTML = slider4.value;


slider1.oninput = function() {
  output1.innerHTML = this.value;
}
slider2.oninput = function() {
  output2.innerHTML = this.value;
}
slider3.oninput = function() {
  output3.innerHTML = this.value;
}
slider4.oninput = function() {
  output4.innerHTML = this.value;
}
</script>
';

?> 
</body>
</html>