<?php
require 'config.php';


$dev= $thr="";
//Device
//DHT1,DHT2
if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $dev = test_input($_POST["dev"]);
        $thr = test_input($_POST["thr"]);
        
        // Create connection
        $conn = new mysqli($servername, $userID, $pass, $database);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } ;
        $sql="UPDATE myesp SET THRESHOLD='".$thr."' WHERE DEVICEID='".$dev."'";
    
        if ($conn->query($sql) === TRUE) {
            echo "New THR record created successfully";
            //echo $dev;
            //echo $thr;
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