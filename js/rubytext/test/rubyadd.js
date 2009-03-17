/*
Always need jquery.js!
*/

$( function() {
	var cin = $( '#stdin' ).focus( function( e ) {
		$( this ).select();
	} );
	var cout = $( '#stdout' );
	var ios = $( '.stdio' );

	function reserveWindow() {
		var winH = jQuery.browser.msie ? document.documentElement.clientHeight : window.innerHeight;
		var modifier;

		if( jQuery.browser.msie )
			modifier = 200;
		else if( jQuery.browser.Gecko )
			modifier = 250;
		else if( jQuery.browser.Opera )
			modifier = 250;
		else if( jQuery.browser.WebKit )
			modifier = 250;
		else
			modifier = 250;

		ios.each( function() {
			$( this ).height( winH - modifier );
		} );
	}

	$( window ).resize( reserveWindow );

	reserveWindow();

	$( '#testcase' ).text( $( '#testcase' ).text() + Rubytext.version );

	$( '#generate' ).click( function( e ) {
		var temp = new Rubytext( cin.val() ).createTag( 'ja.Hiragana' );
		if( temp )
			cin.val( temp );
		else
			alert( 'No string was found!' );
	} );

	$( '#deltag' ).click( function( e ) {
		var temp = new Rubytext( cin.val() ).removeTag();
		if( temp )
			cin.val( temp );
		else
			alert( 'No string was found!' );
	} );

	$( '#deltext' ).click( function( e ) {
		var temp = new Rubytext( cin.val() ).removeTag().clearText( 'ja.Hiragana' );
		if( temp )
			cout.html( '<pre>' + temp + '</pre>' );
		else
			alert( 'No string was found!' );
	} );

	$( '#delbase' ).click( function( e ) {
		var temp = new Rubytext( cin.val() ).removeTag().clearBase( 'ja.Hiragana' );
		if( temp ) {
			cout.html( '<pre>' + temp + '</pre>' );
		} else {
			alert( 'Nothing to do!' );
		}
	} );

	$( '#test' ).click( function( e ) {
		var temp = new Rubytext( cin.val() ).removeTag().clearBase( 'ja.Hiragana' ).rubyTransform();
		if( temp ) {
			cout.html( '<pre>' + temp + '</pre>' );
		} else {
			alert( 'Nothing to do!' );
		}
	} );

	$( '#ppre' ).click( function( e ) {
		cout.html( '<pre>' + cin.val() + '</pre>' );
	} );

	$( '#wpre' ).click( function( e ) {
		cout.html( cin.val() );
	} );

	$( '#clean' ).click( function( e ) {
		cout.empty();
	} );

} );
