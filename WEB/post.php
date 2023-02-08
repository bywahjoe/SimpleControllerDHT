<?php
require 'config.php';


$dev= $d1= $d2= $d3="";
//Device
//DHT1,DHT2
//THRESS
if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $dev = test_input($_POST["dev"]);
        $d1 = test_input($_POST["d1"]);
        $d2 = test_input($_POST["d2"]);
        $d3 = test_input($_POST["d3"]);
        
        // Create connection
        $conn = new mysqli($servername, $userID, $pass, $database);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } ;
        $sql="UPDATE myesp SET DHT1='".$d1."',DHT2='".$d2."',THRESHOLD='".$d3."' WHERE DEVICEID='".$dev."'";
    
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>