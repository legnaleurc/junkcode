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

$query = "SELECT `title` FROM $TABLE WHERE `title`='$args[title]'";
if( mysql_num_rows( mysql_query( $query, $link ) ) > 0 ) {
	$query = "DELETE FROM $TABLE WHERE `title`='$args[title]'";
	$result = mysql_query( $query, $link );
	if( !$result ) {
		returnError( mysql_error( $link ) );
	}
}
?>
