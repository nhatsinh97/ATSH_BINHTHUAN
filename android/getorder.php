
<?php
date_default_timezone_set('Asia/Ho_Chi_Minh');
$connect = mysqli_connect('127.0.0.1:3306','u697720360_traibinhthuan','123456789Aa','u697720360_demo');
	mysqli_set_charset($connect,"utf8");

	$date = date('Y/m/d h:i:s a', time());
	       
			$notes = $_POST["notes"];
			$notes2 = $_POST["notes2"];




			$json = '[{"id":"114","quantity":"1","price":"83600","discount":"0"}]';
			$result = array();
			$sql= "SELECT * FROM `cms_orders` WHERE `ID`=(SELECT MAX(`ID`) FROM cms_orders)";
			
			
		$rows = mysqli_query($connect,$sql);
				$count = mysqli_num_rows($rows);

				while($row1 = mysqli_fetch_assoc($rows)){

					$result[] = ($row1);
					
				}



			if($rows == true ){
			
				$arra =[
                    'success'=>true,
                    'message' => "Đăng nhập thành công",
                    'result' => $result
                   
                    
				];
				print_r(json_encode($arra));
			}
			else{
				$arra =[
                    'success'=>false,
                    'message' => $sql
                    
				];
				print_r(json_encode($arra));	
			}

				
		

	?>
		

