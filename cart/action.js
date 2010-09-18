$( function() {

	function newRow( data ) {
		return $( '<tr/>' ).append( $( '<td/>' ).append( $( '<input/>' ).attr( {
				type: 'checkbox'
			} ).addClass( 'check' ) ) ).append( $( '<td/>' ).addClass( 'title' ).append( $( '<a/>' ).attr( {
			href: data.uri,
			rel: 'external'
		} ).text( data.title ).click( function( event ) {
			event.preventDefault();
			window.open( $( this ).attr( 'href' ), '_blank' );
		} ) ) ).append( $( '<td/>' ).addClass( 'date' ).text( data.date ) ).append( $( '<td/>' ).append( $( '<input/>' ).attr( {
			type: 'button',
			value: 'Delete'
		} ).click( function( event ) {
			var self = $( this );
			var key = self.parent().siblings( '.title' ).text();
			jQuery.post( 'delete.php', {
				title: key
			}, function( data, textStatus ) {
				if( textStatus == 'success' ) {
					self.parent().parent().remove();
				} else {
					cerr( data );
				}
			}, 'json' );
		} ) ) );
	}

	function cerr( msg ) {
		$( '#stderr' ).text( msg );
	}

	jQuery.getJSON( 'load.php', function( data, textStatus ) {
		if( textStatus == 'success' ) {
			$( data ).each( function( index, row ) {
				$( '#cart' ).append( newRow( row ) );
			} );
		} else {
			cerr( data );
		}
	} );

	$( '#submit' ).click( function() {
		var args = {
			title: $( '#title' ).val(),
			uri: $( '#uri' ).val(),
			date: $( '#date' ).val()
		};
		if( args.title == '' || args.uri == '' ) {
			cerr( 'No empty field(s)' );
			return;
		}
		if( !/\d\d\d\d\/\d\d\/\d\d/.test( args.date ) ) {
			cerr( 'Wrong date: ' + args.date );
			return;
		}
		jQuery.post( 'save.php', args, function( data, textStatus ) {
			if( textStatus == 'success' ) {
				var done = false;
				$( '#cart' ).children().each( function( index, self ) {
					self = $( self );
					if( self.children().filter( '.date' ).text() > args.date ) {
						self.before( newRow( args ) );
						done = true;
						return false;
					}
				} );
				if( !done ) {
					$( '#cart' ).append( newRow( args ) );
				}
				$( '#stdin input[type=text]' ).val( '' );
			} else {
				cerr( data );
			}
		}, 'json' );
	} );

	$( '#stdin input[type=text]' ).focus( function() {
		this.select();
	} );

} );
