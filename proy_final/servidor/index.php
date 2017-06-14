<?php
	$str = file_get_contents('./cqrty.json');
	$json = json_decode($str,true);
	$alarma_val = $json['alarm'];
	$garaje_val = $json['garaje'];
?>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>Control Web a Alarma</title>
	<script src="https://code.jquery.com/jquery-2.1.4.min.js"></script>
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js"></script>
	<link href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css" rel="stylesheet">
	<link rel="stylesheet" href="//maxcdn.bootstrapcdn.com/font-awesome/4.3.0/css/font-awesome.min.css">
	<link rel="stylesheet" href="style.css">

</head>
<body>
	<center>
		<div class="row" style="margin-top: 20px;">
			<div class="light-status well col-md-8 col-md-offset-2">
				<h1>SKY-CQRTY</h1>
				<br/>
				<div>
					<text style="bottom:15px;position: relative;">Alarma</text>
					<label class="switch">
						<input id="alarma" type="checkbox" <?php if(!empty($alarma_val)) echo checked; ?>>
						<div class="slider round"></div>
					</label>
					<br />
				</div>	
				<div>
					<text style="bottom:15px;position: relative;">Garaje</text>
					<label class="switch">
						<input id="garaje" type="checkbox" <?php if(!empty($garaje_val)) echo checked; ?>>
						<div class="slider round"></div>
					</label>
					<br />
				</div>
				<br />
				<input class="btn btn-info btn-block btn-lg" type="button" onclick="update()" value="Actualizar">


			</div>
		</div>
	</center>
	<script type="text/javascript">
		function update(){
			console.log("to update");
			console.log("alarm "+ $('#alarma').prop('checked'));
			console.log("garaje "+ $('#garaje').prop('checked'));
			$.ajax({
				type: "POST",
				url: "update.php",
				data: {
					alarm:$('#alarma').prop('checked'),
					garaje:$('#garaje').prop('checked')
				}
			}).done(function(msg){
				alert(msg);
			})
		}
		
	</script>
	
</body>
</html>