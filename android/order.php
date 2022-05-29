
<?php
date_default_timezone_set('Asia/Ho_Chi_Minh');
$connect = mysqli_connect('127.0.0.1:3306','u697720360_traibinhthuan','123456789Aa','u697720360_demo');
	mysqli_set_charset($connect,"utf8");

	$date = date('Y/m/d h:i:s a', time());
	       
			$notes = $_POST["notes"];
			$notes2 = $_POST["notes2"];
			$output_code = $_POST["output_code"];

			$json = '[{"id":"114","quantity":"1","price":"83600","discount":"0"}]';
			$result = array();
			$sql= "INSERT INTO `cms_orders`(`output_code`, `customer_id`, `store_id`, `sell_date`, `notes`, `notes2`, `url`, `url2`, `url3`,`payment_method`,`total_price`,`total_origin_price`,`coupon`,`customer_pay`,`total_money`,`total_quantity`,`lack`,`detail_order`,`order_status`,`deleted`,`created`,`updated`,`user_init`,`user_upd`,`sale_id`) VALUES ('$output_code',5,30, '$date','$notes','$notes2','','','',1,200,'',0,20,0,5,0,'$json',1,0, '$date', '$date',20,0,20)";
			
			
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
		

