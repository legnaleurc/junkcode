$( function() {

	// JavaScript 1.8.5
	if ( !Function.prototype.bind ) {
		Function.prototype.bind = function( obj ) {
			var slice = [].slice, args = slice.call(arguments, 1), self = this, nop = function () {
			}, bound = function () {
				return self.apply( this instanceof nop ? this : ( obj || {} ), args.concat( slice.call(arguments) ) );
			};

			nop.prototype = self.prototype;

			bound.prototype = new nop();

			return bound;
		};
	}

	function cerr( msg ) {
		$( '#stderr' ).show().text( msg ).fadeOut( 5 );
	}

	function Row( data ) {
		function openEdit( self ) {
			// this is text label
			self.hide().next().width( self.parent().width() ).val( self.text() ).show().select();
		}
		function closeEdit( self ) {
			// this is input field
			self.hide().prev().show();
		}
		function saveEdit( self, key ) {
			self.prev().text( self.val() );
			var args = {
				title: self.parent().parent().find( '.title' ).text()
			};
			args[key] = self.val();
			jQuery.post( 'save.cgi', args, function( data, textStatus ) {
				if( textStatus != 'success' ) {
					cerr( data );
				}
			} );
		}

		// container element
		this.element = $( '<tr />' );

		// checkbox cell
		this.selector = $( '<td><input type="checkbox" class="check" /></td>' );

		// title cell
		this.title = $( '<td class="title"></td>' );
		this.link = $( '<a rel="external" />' ).click( function( event ) {
			event.preventDefault();
			window.open( $( this ).attr( 'href' ), '_blank' );
		} ).attr( 'href', data.uri ).text( data.title );
		this.title.append( this.link );

		// vendor cell
		this.vendorText = $( '<span />' ).text( data.vendor );
		this.vendor = $( '<td class="vendor" />' ).dblclick( openEdit.bind( null, this.vendorText ) );
		this.vendorEdit = $( '<input type="text" style="display: none;" />' ).blur( function() {
			var self = $( this );
			saveEdit( self, 'vendor' );
			closeEdit( self );
		} );
		this.vendor.append( this.vendorText ).append( this.vendorEdit );

		// date cell
		this.dateText = $( '<span />' ).text( data.date );
		this.date = $( '<td class="date" />' ).dblclick( openEdit.bind( null, this.dateText ) );
		this.dateEdit = $( '<input type="text" style="display: none;" />' ).blur( function() {
			var self = $( this );
			if( self.val() != self.prev().text() && /^\d\d\d\d\/\d\d\/\d\d$/.test( self.val() ) ) {
				saveEdit( self, 'date' );
			}
			closeEdit( self );
		} );
		this.date.append( this.dateText ).append( this.dateEdit );

		this.element.append( this.selector ).append( this.title ).append( this.vendor ).append( this.date );
	}

	Row.prototype.getElement = function() {
		return this.element;
	};

	var todoList = [];
	var doneList = [];

	// initialize table
	jQuery.getJSON( 'load.cgi', function( data, textStatus ) {
		if( textStatus != 'success' ) {
			cerr( data );
			return;
		}
		var cart = $( '#todo .cart' ).empty();
		var done = $( '#done .cart' ).empty();
		$( data ).each( function( index, row ) {
			var tmp = new Row( row );
			if( row.done == 0 ) {
				todoList.push( tmp );
				cart.append( tmp.getElement() );
			} else {
				doneList.push( tmp );
				done.append( tmp.getElement() );
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
		$( '#todo .check' ).attr( 'checked', self.is( ':checked' ) );
	} );

	$( '#select-all-done' ).change( function( event ) {
		var self = $( this );
		$( '#done .check' ).attr( 'checked', self.is( ':checked' ) );
	} );

	$( 'a[rel=external]' ).click( function( event ) {
		event.preventDefault();
		window.open( $( this ).attr( 'href' ), '_blank' );
	} );

	$( '#page-body' ).tabs();

} );
