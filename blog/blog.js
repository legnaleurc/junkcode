// Depends on prototype.js

var Blog = {

	targetLink: function( id, dft, ct ) {
		var toggle = $( id );
		if( toggle.length ){
			throw new Error( 'There is no element matched `' + id + '\' in this DOM!' );
		}

		var flag = true;
		toggle.text( dft ).click( function( e ) {
			e.preventDefault();
			$( this ).text( flag ? ct : dft );
			flag = !flag;
		} );

		$( 'a[rel="external"]' ).click( function( e ) {
			if( flag ) {
				e.preventDefault();
				window.open( $( this ).attr( 'href' ), '_blank' );
			}
		} );
	},

	/**
	 * @brief Hide blocks
	 * @param String selector CSS selector of blocks.
	 * @param String collapsed Collapsed element.
	 * @param String expanded Expanded element.
	 * @throw TypeError If any argument is wrong. Strong safety.
	 */
	hideBlocks: function( selector, collapsed, expanded ) {
		var posts = $( selector );
		if( posts.length == 0 ) {
			throw new Error( 'There is no element matched `' + selector + '\' in this DOM!' );
		}
		if( !( collapsed && expanded ) ) {
			throw new TypeError( 'Invalid argument(s).' );
		}

		posts.each( function( index_, post ) {
			var more = $( collapsed );

			var less = $( expanded ).hide();

			more.click( function( e ) {
				$( [ post, this, less ] ).toggle();
			} );

			less.click( function( e ) {
				$( [ post, this, less ] ).toggle();
			} );

			$( this ).before( more ).after( less ).hide();
		} );
	},
	
	TagCloud: {
		
		cloudMin: 1,
		minFontSize: 10,
		maxFontSize: 20,
		minColor: [ 0xCC, 0xCC, 0xFF ],
		maxColor: [ 0x99, 0x99, 0xFF ],
		showCount: false,
// 		shuffle: true,

// 		randomShuffle: function( hash ) {
// 			var result = new Hash();
// 			var keys = hash.keys();
// 			for( var i = 0, r, temp, len = keys.length; i < len; ++i ) {
// 				do {
// 					r = Math.floor( Math.random() * len );
// 				} while( r == i );
// 				temp = keys[i];
// 				keys[i] = keys[r];
// 				keys[r] = temp;
// 			}
// 			keys.each( function( key ) {
// 				result.set( key, hash.get( key ) );
// 			} );
// 			return result;
// 		},
		
		generate: function( tags ) {
			function helper( a, b, i, x ) {
				if( a > b ) {
					return a - Math.floor( Math.log( i ) * ( a - b ) / Math.log( x ) );
				} else {
					return Math.floor( Math.log( i ) * ( b - a ) / Math.log( x ) + a );
				}
			}
			
			// 標籤的大小數量, 取得標籤的數量後做unique再取陣列長度
			var c = [];
			tags.each( function( key, value ) {
				if( jQuery.inArray( value, c ) < -1 ) {
					c.push( value );
				}
			} );
			var labelCount = c.length;
			
			// color的rgb
			c = new Array( 3 );
			
			// 標籤數量底限
			var ta = this.cloudMin - 1;
			// 大小和顏色的總級數
			var tz = labelCount - this.cloudMin;
			// 放置標籤雲的元素
			var lc2 = $( '#labelCloud' );
			// 清單元素
			var ul = $( '<ul class="label-cloud"/>' );
			
			tags.each( function( key, value ) {
				// 當標籤文章數小於最底限時, 跳過這次的迭代( 由於每次的迭代都是function, 用return取代continue )
				if( value < Blog.TagCloud.cloudMin ) {
					return true;
				}
				// 取得顏色
				c.each( function( index ) {
					this = helper( Blog.TagCloud.minColor[index], Blog.TagCloud.maxColor[index], value - ta, tz );
				} );
				// 建立連結
				var a = $( '<a/>' ).attr( {
					href: [ location.protocol, null, location.host, 'search', 'label', encodeURIComponent( key ) ].join( '/' ),
					title: value + ' Post' + ( value > 1 ? 's' : '' ) + ' in ' + key
				} ).css( {
					color: 'rgb( ' + c.join( ', ' ) + ' )'
				} ).text( key );
				// 取得字體大小
				var fs = helper( Blog.TagCloud.minFontSize, Blog.TagCloud.maxFontSize, value - ta, tz );
				// 建立清單項目
				var li = $( '<li/>' ).css( {
					fontSize: fs + 'px',
					lineHeight: '1em'
				} ).append( a );
				// 項目加入連結
				// 如果設定顯示篇數的話
				if( Blog.TagCloud.showCount ) {
					// 建立行內型元素
					var span = $( '<span class="label-count">(' + value + ')</span>' );
					// 加入項目(在連結元素之後)
					li.append( span );
				}
				// 清單加入項目
				ul.append( li ).append( ' ' );
				// 間隔字元(美化)
			} );

			// 加入清單
			lc2.append( ul );
		}

	},

	highlighting: function() {
		dp.SyntaxHighlighter.ClipboardSwf = 'http://career.ccu.edu.tw/~legnaleurc/library/dp.SyntaxHighlighter/Scripts/clipboard.swf';
		dp.SyntaxHighlighter.BloggerMode();
		dp.SyntaxHighlighter.HighlightAll( 'code' );
	},

	Media: {
		active: true,
		
		trigger: function() {
			$( 'a.Media' ).click( Blog.Media.handler );
			$( 'a.YouTube' ).click( Blog.Media.youTube );
			$( 'a.GoogleVideo' ).click( Blog.Media.googleVideo );
		},
		
		youTube: function( e ) {
			if( Blog.Media.active ) {
				e.preventDefault();
				var self = $( this );
				var url = self.attr( 'href' );
				var temp = url.match( /http:\/\/tw\.youtube\.com\/watch\?(\w)=([\w-]+)/ );
				url = 'http://www.youtube.com/' + temp[1] + '/' + temp[2] + '&hl=zh_TW&fs=1';

				self.before( $( '\
					<object width="425" height="344">\
						<param name="movie" value="' + url + '" />\
						<param name="allowFullScreen" value="true" />\
						<embed src="' + url + '" type="application/x-shockwave-flash" allowFullScreen="true" width="425" height="344" />\
					</object>\
				' ) ).remove();
			}
		},
		
		googleVideo: function( e ) {
			if( Blog.Media.active ) {
				e.preventDefault();
				var self = $( this );
				var url = self.attr( 'href' );
				var temp = url.match( /http:\/\/video\.google\.com\/videoplay\?(\w+)=(.+)/ );
				url = 'http://video.google.com/googleplayer.swf?' + temp[1] + '=' + temp[2] + '&fs=true';

				self.before( $( '<embed allowFullScreen="true" src="' + url + '" type="application/x-shockwave-flash" />').width( self.attr( 'width' ) ).height( self.attr( 'height' ) ) ).remove();
			}
		},

		handler: function( e ) {
			if( Blog.Media.active ) {
				e.preventDefault();
				var self = $( this );
				var attr = {
					value: self.attr( 'href' ),
					width: self.attr( 'width' ),
					height: self.attr( 'height' ),
					uiMode: self.attr( 'uimode' ),
					autoStart: self.attr( 'autostart' )
					type: Blog.Media.getMIME( attr.value )
				};
				var media = $( '\
					<object data="' + attr.value + '" type="' + attr.type + '">\
						<param name="Filename" value="' + attr.value + '" />\
						<param name="src" value="' + attr.value + '" />\
						<param name="Scale" value="Aspect">\
					</object>\
				' ).width( attr.width ).height( attr.height );
				if( attr.uiMode ) {
					media.append( $( '<param name="uiMode" value="' + attr.uiMode + '" />' ) );
				}
				if( attr.autoStart ) {
					media.append( $( '<param name="AutoPlay" value="' + attr.autoStart + '" />' ) );
					media.append( $( '<param name="AutoStart" value="' + attr.autoStart + '" />' ) );
				}
				self.before( media ).remove();
			}
		},

		getMIME: function( S ) {
			var ext = S.match( /\.[a-z0-9]+$/i )[0].toLowerCase();
			switch( ext ) {
				case '.mp3':
					return 'audio/mpeg';
				case '.mpg':
					return 'video/mpeg';
				default:
					return '*/*';
			}
		}
	}

};
