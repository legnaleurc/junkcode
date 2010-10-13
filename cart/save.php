<?php
require_once 'config.inc';
require_once 'lib.inc';

$link = mysql_connect( $HOST, $USER, $PASSWORD );
if( !$link ) {
	returnError( mysql_error( $link ) );
}

if( !mysql_select_db( $DATABASE, $link ) ) {
	returnError( mysql_error( $link ) );
}

foreach( $_POST as $key => $value ) {
	$args[$key] = mysql_real_escape_string( $value );
}

$query = "SELECT * FROM $TABLE WHERE `title`='$args[title]'";
$result = mysql_query( $query, $link );
if( !$result ) {
	returnError( mysql_error( $link ) );
}
if( mysql_num_rows( $result ) > 0 ) {
	$query = "UPDATE $TABLE SET uri='$args[uri]',date='$args[date]' WHERE title='$args[title]'";
	$result = mysql_query( $query, $link );
} else {
	$query = "INSERT INTO $TABLE (title,uri,date,done) VALUES('$args[title]','$args[uri]','$args[date]',0)";
	$result = mysql_query( $query, $link );
}
?>
