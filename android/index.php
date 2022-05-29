<?php

$connect = mysqli_connect('127.0.0.1:3306','u697720360_traibinhthuan','123456789Aa','u697720360_demo');
	mysqli_set_charset($connect,"utf8");

		
			$tk = $_POST["username"];
			$mk = ($_POST["password"]);
		 
			$result = array();
			
		$rows = mysqli_query($connect,"select * from  cms_users where username = '".$tk."' and password = '".$mk."'");
				$count = mysqli_num_rows($rows);

				while($row1 = mysqli_fetch_assoc($rows)){

					$result[] = ($row1);
				}



			if($count == 1 ){
			
				$arra =[
                    'success'=>true,
                    'message' => "Đăng nhập thành công",
                     "result" => $result
                    
                    
				];
				print_r(json_encode($arra));
			}
			else{
				$arra =[
                    'success'=>false,
                    'message' => "select * from  cms_users where username = ".$tk." and password = ".$mk.""
                    
				];
				print_r(json_encode($arra));	
			}

				
		

	?>