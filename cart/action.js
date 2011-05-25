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

	function bind( fn ) {
		var args = Array.prototype.slice.call( arguments, 1 );
		return function() {
			fn.apply( this, args.concat( Array.prototype.slice.call( arguments ) ) );
		};
	}

	function cerr( msg ) {
		$( '#stderr' ).show().text( msg ).fadeOut( 5 );
	}

	var isWritable = true;

	function Table( selector ) {
		this.items = [];
		this.view = $( selector ).empty();
	}

	Table.prototype.append = function( row ) {
		this.items.push( row );
		this.view.append( row.getElement() );
	};

	Table.prototype.remove = function( index ) {
		var itemTitle = this.items[index].getTitle();
		jQuery.post( 'delete.cgi', {
			title: itemTitle
		}, function( data, textStatus ) {
			if( textStatus == 'success' ) {
				this.items.splice( index, 1 )[0].getElement().remove();
			} else {
				cerr( data );
			}
		}.bind( this ), 'json' );
	};

	Table.prototype.at = function( index ) {
		return this.items[index];
	};

	Table.prototype.size = function() {
		return this.items.length;
	};

	Table.prototype.find = function( row, compare ) {
		if( compare === undefined ) {
			compare = function( l, r ) {
				if( l.getDate() == r.getDate() ) {
					if( l.getTitle() == r.getTitle() ) {
						return 0;
					}
					return ( l.getTitle() < r.getTitle() ) ? -1 : 1;
				}
				return ( l.getDate() < r.getDate() ) ? -1 : 1;
			};
		}
		function binarySearch( row, list, begin, end ) {
			var middle = Math.floor( ( begin + end ) / 2 );
			var that = list[middle];
			var tmp = compare( row, that );
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
		return ( this.items.length == 0 ) ? {
			found: false,
			index: 0
		} : binarySearch( row, this.items, 0, this.items.length );
	};

	Table.prototype.take = function( index ) {
		var taken = this.items.splice( index, 1 )[0];
		taken.getElement().detach();
		return taken;
	};

	Table.prototype.insert = function( index, row ) {
		if( this.items.length == 0 ) {
			this.append( row );
		} else if( index >= this.items.length ) {
			this.items[ this.items.length - 1 ].getElement().after( row.getElement() );
			this.items.push( row );
		} else if( index > 0 ) {
			this.items[index].getElement().before( row.getElement() );
			this.items.splice( index, 0, row );
		} else {
			this.items[0].getElement().before( row.getElement() );
			this.items.splice( 0, 0, row );
		}
		return this;
	};

	function Row( data ) {
		function openEdit( parent, label, input ) {
			input.width( parent.width() ).val( label.hide().text() ).show().select();
		}
		function closeEdit( label, input ) {
			label.show();
			input.hide();
		}
		function saveEdit( label, input, key, field ) {
			if( label.text() == input.val() ) {
				return;
			}
			label.text( input.val() );
			var args = {
				title: key
			};
			args[field] = input.val();
			jQuery.post( 'save.cgi', args, function( data, textStatus ) {
				if( textStatus != 'success' ) {
					cerr( data );
				}
			} );
		}

		// container element
		this.element = $( '<tr />' );

		// checkbox cell
		this.selector = $( '<td></td>' );
		this.checkbox = $( '<input type="checkbox" class="check" />' );
		this.selector.append( this.checkbox );

		// title cell
		this.title = $( '<td class="title"></td>' );
		this.link = $( '<a rel="external" />' ).click( function( event ) {
			event.preventDefault();
			window.open( $( this ).attr( 'href' ), '_blank' );
		} ).attr( 'href', data.uri ).text( data.title );
		this.title.append( this.link );

		// vendor cell
		this.vendor = $( '<td class="vendor" />' );
		this.vendorText = $( '<span />' ).text( data.vendor );
		this.vendorEdit = $( '<input type="text" style="display: none;" />' ).blur( function() {
			saveEdit( this.vendorText, this.vendorEdit, this.title.text(), 'vendor' );
			closeEdit( this.vendorText, this.vendorEdit );
		}.bind( this ) );
		this.vendor.dblclick( bind( openEdit, this.vendor, this.vendorText, this.vendorEdit ) );
		this.vendor.append( this.vendorText ).append( this.vendorEdit );

		// date cell
		this.dateText = $( '<span />' ).text( data.date );
		this.date = $( '<td class="date" />' );
		this.dateEdit = $( '<input type="text" style="display: none;" />' ).blur( function() {
			if( /^\d\d\d\d\/\d\d\/\d\d$/.test( this.dateEdit.val() ) ) {
				saveEdit( this.dateText, this.dateEdit, this.title.text(), 'date' );
			}
			closeEdit( this.dateText, this.dateEdit );
		}.bind( this ) );
		this.date.dblclick( bind( openEdit, this.date, this.dateText, this.dateEdit ) );
		this.date.append( this.dateText ).append( this.dateEdit );

		this.element.append( this.selector ).append( this.title ).append( this.vendor ).append( this.date );
	}

	Row.prototype.getElement = function() {
		return this.element;
	};

	Row.prototype.isChecked = function() {
		return this.checkbox.is( ":checked" );
	};

	Row.prototype.setChecked = function( checked ) {
		this.checkbox.attr( 'checked', checked );
		return this;
	};

	Row.prototype.getTitle = function() {
		return this.title.text();
	};

	Row.prototype.getDate = function() {
		return this.dateText.text();
	};

	var carts = [ new Table( '#phase-0 .cart' ), new Table( '#phase-1 .cart' ), new Table( '#phase-2 .cart' ), new Table( '#phase-3 .cart' ) ];

	// initialize table
	jQuery.getJSON( 'load.cgi', function( data, textStatus ) {
		if( textStatus != 'success' ) {
			cerr( data );
			return;
		}
		$( data ).each( function( index, row ) {
			var tmp = new Row( row );
			carts[row.done].append( tmp );
		} );
	} );

	$( '#button-delete' ).click( function( ev ) {
		jQuery.each( carts, function( index, list ) {
			for( var i = list.size() - 1; i >= 0; --i ) {
				if( list.at( i ).isChecked() ) {
					list.remove( i );
				}
			}
		} );
	} );

	function setItemPhase( phase ) {
		var movingItem = [];
		for( var i = 0; i < carts.length; ++i ) {
			if( i == phase ) {
				continue;
			}
			// this loop will remove items from array
			for( var j = carts[i].size() - 1; j >= 0; --j ) {
				if( !carts[i].at( j ).isChecked() ) {
					continue;
				}
				movingItem.push( carts[i].take( j ).setChecked( false ) );
			}
		}
		for( i = 0; i < movingItem.length; ++i ) {
			var result = carts[phase].find( movingItem[i] );
			carts[phase].insert( result.index, movingItem[i] );

			// do query
			if( !isWritable ) {
				continue;
			}
			jQuery.post( 'save.cgi', {
				title: movingItem[i].getTitle(),
				done: phase
			}, function( data, textStatus ) {
				if( textStatus != 'success' ) {
					cerr( data );
					return;
				}
			}, 'json' );
		}
	}

	$( '#button-to-phase-0' ).click( function() {
		setItemPhase( 0 );
	} );

	$( '#button-to-phase-1' ).click( function() {
		setItemPhase( 1 );
	} );

	$( '#button-to-phase-2' ).click( function() {
		setItemPhase( 2 );
	} );

	$( '#button-to-phase-3' ).click( function() {
		setItemPhase( 3 );
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
			var row = new Row( args );
			var result = todoList.find( row );
			if( result.found ) {
				var taken = todoList.take( result.index );
				taken.getElement().remove();
			}
			todoList.insert( result.index, row );
			// clear input fields
			$( '#stdin input[type=text]' ).val( '' );
		}, 'json' );
	} );

	$( '#stdin input[type=text]' ).focus( function() {
		this.select();
	} );

	// select all setting
	for( var i = 0; i < 4; ++i ) {
		$( '#phase-' + i + ' .select-all' ).change( bind( function( index, event ) {
			$( '#phase-' + index + ' .check' ).attr( 'checked', $( this ).is( ':checked' ) );
		}, i ) );
	}

	$( 'a[rel=external]' ).click( function( event ) {
		event.preventDefault();
		window.open( $( this ).attr( 'href' ), '_blank' );
	} );

	$( '#page-body' ).tabs();

} );
