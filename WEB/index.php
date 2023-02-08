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
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $t1 = $row["DEVICEID"];
        $t2 = $row["DHT1"];
        $t3 = $row["DHT2"];
        $t4 = $row["THRESHOLD"];
        
        echo ' 
            <b>DEVICEID -> ' . $t1 . '</b> <br>
            <b>LAST DHT1-> ' . $t2 . ' 째C</b> <br>
            <b>LAST DHT2-> ' . $t3 . ' 째C</b> <br>
            <b>LAST THRESHOLD-> ' . $t4 . '</b> <br>
                 
              ';
    }       
    $result->free();
}

$conn->close();
echo '
<br><br>
    <form action="/setThreshold.php" method="post">

  <label for="thres">Set New Threshold (between 20 and 50): </label>
  <br>
  <p>-New Threshold: <span id="slid"></span></p>
  <input type="range" id="threshold" name="thr" min="20" max="50" value='.$t4.'>
  <input type="hidden" name="dev" value='.$t1.'>
  <input type="submit" value="Submit">
</form>';
echo '<script>
var slider = document.getElementById("threshold");
var output = document.getElementById("slid");
output.innerHTML = slider.value;

slider.oninput = function() {
  output.innerHTML = this.value;
}
</script>
';

?> 
</body>
</html>