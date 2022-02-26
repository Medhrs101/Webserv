<!DOCTYPE html>
<html>
<meta charset="utf-8">
<title>Forms action page</title>

<body class="w3-container">
    <h1>Submitted Form Data</h1>
    <h2>Your input was received as:</h2>
    <div class="w3-container w3-large w3-border" style="word-wrap:break-word">
        <?php 
            if ($_SERVER['REQUEST_METHOD'] == 'GET') {
                echo "<h1>GET</h1>";
                $name = $_GET['fname'];
                $lname = $_GET['lname'];
                echo "first name: " . $name . "<br/>" , ">> last name : " . $lname;
            }
        ?>
        <?php 
            if ($_SERVER['REQUEST_METHOD'] == 'POST') {
                echo "<h1>POST</h1>";
                $name = $_POST['fname'];
                $lname = $_POST['lname'];
                echo "first name: " . $name . "<br/>" , ">> last name : " . $lname;
                // var_dump($_FILES);
            }
        ?>
    <p>The server has processed your input and returned this answer.</p>

    <div class="w3-panel w3-pale-yellow w3-leftbar w3-border-yellow">
    </div>

</body>

</html>