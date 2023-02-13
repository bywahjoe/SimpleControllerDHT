<?php
require 'config.php';

$conn = new mysqli($servername, $userID, $pass, $database);
if(!empty($_GET["dev"])){
if ($_SERVER["REQUEST_METHOD"] == "GET"){
    header('Content-Type: application/json; charset=utf-8');
	$devname=$_GET["dev"];
	if ($conn->connect_error) {
    	die("Connection failed: " . $conn->connect_error);
	};

	$sql = "SELECT * FROM myesp WHERE DEVICEID='".$devname."'";

	$result = $conn->query($sql);
    $minDHT1=$maxDHT1=$minDHT2=$maxDHT2=0;
    $mode="";
    $valDHT1=$valDHT2=0.0;
	while ($row=$result->fetch_assoc()) {
		$mode=$row["MODE"];
		$valDHT1=$row["DHT1"];
		$valDHT2=$row["DHT2"];
		$valHUMID1=$row["HUMID1"];
		$valHUMID2=$row["HUMID2"];
		$minDHT1=$row["vminDHT1"];
		$maxDHT1=$row["vmaxDHT1"];
        $minDHT2=$row["vminDHT2"];
        $maxDHT2=$row["vmaxDHT2"];
	}
// 	echo $mode;
// 	$arr=array("mode"=>1);
    $arr=array();
    if(!empty($_GET["raw"])){
       $arr=array("mode"=>$mode,"dht1"=>array("valDHT1"=>$valDHT1,"valHUMID1"=>$valHUMID1,"minDHT1"=>$minDHT1,"maxDHT1"=>$maxDHT1),"dht2"=>array("valDHT2"=>$valDHT2,"valHUMID2"=>$valHUMID2,"minDHT2"=>$minDHT2,"maxDHT2"=>$maxDHT2)); 
        
        
    }else{
	$arr=array("mode"=>$mode,"dht1"=>array("minDHT1"=>$minDHT1,"maxDHT1"=>$maxDHT1),"dht2"=>array("minDHT2"=>$minDHT2,"maxDHT2"=>$maxDHT2));}
    echo json_encode($arr);
    // echo "test";
	$conn->close();
}}else{
    echo '0';
}
?>