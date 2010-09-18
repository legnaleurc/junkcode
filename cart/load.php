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

$query = "SELECT * FROM $TABLE";
$result = mysql_query( $query, $link );
while( $row = mysql_fetch_array( $result, MYSQL_ASSOC ) ) {
	$cart[] = $row;
}

function helper( $l, $r ) {
	if( $l['date'] == $r['date'] ) {
		if( $l['title'] == $r['title'] ) {
			return 0;
		}
		return ( $l['title'] < $r['title'] ) ? -1 : 1;
	}
	return ( $l['date'] < $r['date'] ) ? -1 : 1;
}
usort( $cart, 'helper' );

echo json_encode( $cart );
?>
