<?php
/**
 * @file save.php
 * @param title Game title, use as primary key
 * @param uri Intro page url
 * @param date Release date
 * @param done Is downloaded
 */
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
if( !array_key_exists( 'title', $args ) || !$args['title'] ) {
	returnError( 'Must provide a primary key.' );
}

$query = "SELECT * FROM $TABLE WHERE `title`='$args[title]'";
$result = mysql_query( $query, $link );
if( !$result ) {
	returnError( mysql_error( $link ) );
}
if( mysql_num_rows( $result ) > 0 ) {
	// item exists, update
	foreach( array( 'uri', 'date', 'done' ) as $value ) {
		if( array_key_exists( $value, $args ) ) {
			$tmp[] = sprintf( '%s=\'%s\'', $value, $args[$value] );
		}
	}
	$tmp = implode( ',', $tmp );
	if( $tmp ) {
		$query = "UPDATE $TABLE SET $tmp WHERE title='$args[title]'";
		$result = mysql_query( $query, $link );
	}
} else {
	// new item, insert
	$query = "INSERT INTO $TABLE (title,uri,date,done) VALUES('$args[title]','$args[uri]','$args[date]','$args[done]')";
	$result = mysql_query( $query, $link );
}
?>
