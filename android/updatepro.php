
<?php

$connect = mysqli_connect('127.0.0.1:3306','u697720360_traibinhthuan','123456789Aa','u697720360_demo');
	mysqli_set_charset($connect,"utf8");
	
	
			$code = $_POST["prd_code"];
			$quanty = $_POST["prd_sls"];
			
			$result = array();
			$sql = "UPDATE `cms_products` SET `prd_sls`=".$quanty." WHERE `prd_code`='".$code."'";
			
		$rows = mysqli_query($connect,$sql);
				$count = mysqli_num_rows($rows);

				while($row1 = mysqli_fetch_assoc($rows)){

					$result[] = ($row1);
					
				}



			if($rows == true ){
			
				$arra =[
                    'success'=>true,
                    'message' => "Đăng nhập thành công"
                   
                    
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
		

