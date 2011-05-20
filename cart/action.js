$( function() {

	$( '#page-body' ).tabs();

	function cerr( msg ) {
		$( '#stderr' ).show().text( msg ).fadeOut( 5 );
	}

	function newRow( data ) {
		return $( '<tr/>' ).append( $( '<td/>' ).append( $( '<input/>' ).attr( {
				type: 'checkbox'
			} ).addClass( 'check' ) ) ).append( $( '<td/>' ).addClass( 'title' ).append( $( '<a/>' ).attr( {
			href: data.uri,
			rel: 'external'
		} ).text( data.title ).click( function( event ) {
			event.preventDefault();
			window.open( $( this ).attr( 'href' ), '_blank' );
		} ) ) ).append( $( '<td class="date"/>' ).append( $( '<span/>' ).text( data.date ).dblclick( function( ev ) {
			// this is text label
			var self = $( this );
			self.hide().next().width( self.width() ).val( self.text() ).show().select();
		} ) ).append( $( '<input type="text"/>' ).hide().blur( function( ev ) {
			// this is input field
			var self = $( this );
			if( self.val() != self.prev().text() && /^\d\d\d\d\/\d\d\/\d\d$/.test( self.val() ) ) {
				self.prev().text( self.val() );
				jQuery.post( 'save.cgi', {
					title: self.parent().prev().text(),
					date: self.val()
				}, function( data, textStatus ) {
					if( textStatus != 'success' ) {
						cerr( data );
					}
				} );
			}
			self.hide().prev().show();
		} ) ) );
	}

	// initialize table
	jQuery.getJSON( 'load.cgi', function( data, textStatus ) {
		if( textStatus != 'success' ) {
			cerr( data );
			return;
		}
		var cart = $( '#todo .cart' ).empty();
		var done = $( '#done .cart' ).empty();
		$( data ).each( function( index, row ) {
			if( row.done == 0 ) {
				cart.append( newRow( row ) );
			} else {
				done.append( newRow( row ) );
			}
		} );
	} );

	$( '#button-delete' ).click( function( ev ) {
		var ck = $( '.cart > tr' ).filter( function( index ) {
			return $( '.check', this ).attr( 'checked' );
		} ).each( function( index, ele ) {
			var self = $( this );
			jQuery.post( 'delete.cgi', {
				title: self.children( '.title' ).text()
			}, function( data, textStatus ) {
				if( textStatus == 'success' ) {
					self.remove();
				} else {
					cerr( data );
				}
			}, 'json' );
		} );
	} );

	function removeFromTableByKey( selector, key ) {
		selector += ' > tbody > tr';
		return $( selector ).filter( function( index ) {
			return $( '.title', this ).text() == key;
		} ).remove();
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

	function binarySearch( row, list, begin, end ) {
		var middle = Math.floor( ( begin + end ) / 2 );
		var that = $( list[middle] );
		var tmp = compare( row, {
			date: that.find( '.date' ).text(),
			title: that.find( '.title' ).text()
		} );
		if( tmp < 0 ) {
			if( end - begin == 1 ) {
				return {
					found: false,
					index: middle
				};
			} else {
				return binarySearch( row, list, begin, middle );
			}
		} else if( tmp > 0 ) {
			if( end - begin == 1 ) {
				return {
					found: false,
					index: end
				};
			} else {
				return binarySearch( row, list, middle, end );
			}
		} else {
			return {
				found: true,
				index: middle
			};
		}
	}

	$( '#button-todo' ).click( function( ev ) {
		var ck = $( '#done > tbody > tr' ).filter( function( index ) {
			return $( '.check', this ).attr( 'checked' );
		} ).each( function( index, ele ) {
			var self = $( this );
			var args = {
				title: self.children( '.title' ).text(),
				uri: self.find( 'a' ).attr( 'href' ),
				date: self.children( '.date' ).text(),
				done: 0
			};
			jQuery.post( 'save.cgi', args, function( data, textStatus ) {
				if( textStatus == 'success' ) {
					var list = $( '#todo > tbody > tr' );
					list.filter( function( index ) {
						return $( '.title', this ).text() == args.title;
					} ).remove();
					var result = binarySearch( args, list, 0, list.length );
					if( result.index == list.length ) {
						self.remove().insertAfter( list.last() ).find( '.check' ).attr( 'checked', false );
					} else {
						self.remove().insertBefore( list[result.index] ).find( '.check' ).attr( 'checked', false );
					}
				} else {
					cerr( data );
				}
			}, 'json' );
		} );
	} );

	$( '#button-done' ).click( function( ev ) {
		var ck = $( '#todo > tbody > tr' ).filter( function( index ) {
			return $( '.check', this ).attr( 'checked' );
		} ).each( function( index, ele ) {
			var self = $( this );
			var args = {
				title: self.children( '.title' ).text(),
				uri: self.find( 'a' ).attr( 'href' ),
				date: self.children( '.date' ).text(),
				done: 1
			};
			jQuery.post( 'save.cgi', args, function( data, textStatus ) {
				if( textStatus == 'success' ) {
					var list = $( '#done > tbody > tr' );
					list.filter( function( index ) {
						return $( '.title', this ).text() == args.title;
					} ).remove();
					var result = binarySearch( args, list, 0, list.length );
					if( result.index == list.length ) {
						self.remove().insertAfter( list.last() ).find( '.check' ).attr( 'checked', false );
					} else {
						self.remove().insertBefore( list[result.index] ).find( '.check' ).attr( 'checked', false );
					}
				} else {
					cerr( data );
				}
			}, 'json' );
		} );
	} );

	$( '#submit' ).click( function() {
		var args = {
			title: $( '#title' ).val(),
			uri: $( '#uri' ).val(),
			date: $( '#date' ).val(),
			done: 0,
			vendor: '',
			volume: -1
		};
		if( args.title == '' || args.uri == '' ) {
			cerr( 'No empty field(s)' );
			return;
		}
		if( !/^\d\d\d\d\/\d\d\/\d\d$/.test( args.date ) ) {
			cerr( 'Wrong date: ' + args.date );
			return;
		}
		jQuery.post( 'save.cgi', args, function( data, textStatus ) {
			if( textStatus != 'success' ) {
				cerr( data );
				return;
			}
			var list = $( '#todo > tbody > tr' );
			list.filter( function( index ) {
				return $( '.title', this ).text() == args.title;
			} ).remove();
			var result = binarySearch( args, list, 0, list.length );
			if( result.index == list.length ) {
				newRow( args ).insertAfter( list.last() );
			} else {
				newRow( args ).insertBefore( list[result.index] );
			}
			// clear input fields
			$( '#stdin input[type=text]' ).val( '' );
		}, 'json' );
	} );

	$( '#stdin input[type=text]' ).focus( function() {
		this.select();
	} );

	$( '#select-all-todo' ).change( function( event ) {
		var self = $( this );
		if( self.is( ':checked' ) ) {
			$( '#todo .check' ).attr( 'checked', 'checked' );
		} else {
			$( '#todo .check' ).removeAttr( 'checked' );
		}
	} );

	$( '#select-all-done' ).change( function( event ) {
		var self = $( this );
		if( self.is( ':checked' ) ) {
			$( '#done .check' ).attr( 'checked', 'checked' );
		} else {
			$( '#done .check' ).removeAttr( 'checked' );
		}
	} );

	$( 'a[rel=external]' ).click( function( event ) {
		event.preventDefault();
		window.open( $( this ).attr( 'href' ), '_blank' );
	} );

} );
