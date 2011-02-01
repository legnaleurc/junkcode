$( function() {

	$( '#page-body' ).tabs();

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
		$( '#stderr' ).show().text( msg ).fadeOut( 5 );
	}

	jQuery.getJSON( 'load.php', function( data, textStatus ) {
		if( textStatus != 'success' ) {
			cerr( data );
			return;
		}
		var cart = $( '#todo .cart' ).empty();
		$( data ).each( function( index, row ) {
			cart.append( newRow( row ) );
		} );
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
			if( textStatus != 'success' ) {
				cerr( data );
				return;
			}
			function compare( l, r ) {
				if( l.date == r.date ) {
					if( l.title == r.title ) {
						return 0;
					}
					return ( l.title < r.title ) ? -1 : 1;
				}
				return ( l.date < r.date ) ? -1 : 1;
			}
			function binaryInsert( row, list, b, e ) {
				var m = Math.floor( ( b + e ) / 2 );
				var that = $( list[m] );
				var tmp = compare( row, {
					date: that.find( '.date' ).text(),
					title: that.find( '.title' ).text()
				} );
				if( tmp < 0 ) {
					if( e - b == 1 ) {
						that.before( newRow( row ) );
					} else {
						binaryInsert( row, list, b, m );
					}
				} else if( tmp > 0 ) {
					if( e - b == 1 ) {
						that.after( newRow( row ) );
					} else {
						binaryInsert( row, list, m, e );
					}
				} else {
					that.find( '.title a' ).attr( {
						href: row.uri
					} );
					that.find( '.date' ).text( row.date );
				}
			}
			$( '#todo .cart' ).children().each( function() {
				var self = $( this );
				if( self.find( '.title' ).text() == args.title || self.find( '.uri' ).text() == args.uri ) {
					self.remove();
				}
			} );
			var list = $( '#todo .cart' ).children();
			binaryInsert( args, list, 0, list.length );
			$( '#stdin input[type=text]' ).val( '' );
		}, 'json' );
	} );

	$( '#stdin input[type=text]' ).focus( function() {
		this.select();
	} );

	$( '#select-all-todo, #select-all-done' ).change( function( event ) {
		var self = $( this );
		$( '.check' ).attr( {
			checked: self.attr( 'checked' )
		} );
	} );

	$( 'a[rel=external]' ).click( function( event ) {
		event.preventDefault();
		window.open( $( this ).attr( 'href' ), '_blank' );
	} );

} );
