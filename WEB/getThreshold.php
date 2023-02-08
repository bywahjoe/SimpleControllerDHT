<?php
require 'config.php';

$conn = new mysqli($servername, $userID, $pass, $database);

if ($_SERVER["REQUEST_METHOD"] == "GET"){

	$devname=$_GET["dev"];
	if ($conn->connect_error) {
    	die("Connection failed: " . $conn->connect_error);
	};

	$sql = "SELECT THRESHOLD FROM myesp WHERE DEVICEID='".$devname."'";

	$result = $conn->query($sql);

	while ($row=$result->fetch_assoc()) {
		echo $row["THRESHOLD"];
	}

	$conn->close();
}else{
    echo '0';
        
}
?>