// Depends on jQuery
// Depends on jQuery Media Plugin

var Blog = {

	targetLink: function( param ) {
		var toggle = $( param.id );
		if( toggle.length == 0 ){
			throw new Error( 'There is no element matched `' + param.id + '\' in this DOM!' );
		}

		var flag = true;
		toggle.text( param.dft ).click( function( e ) {
			e.preventDefault();
			$( this ).text( flag ? param.ct : param.dft );
			flag = !flag;
		} );

		$( 'a[rel="external"]' ).bind( 'click.target', function( e ) {
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
	hideBlocks: function( param ) {
		if( !( param.collapsed && param.expanded ) ) {
			throw new TypeError( 'Invalid argument(s).' );
		}
		var posts = $( param.selector );

		posts.each( function( index_, post ) {
			var more = $( param.collapsed );

			// for bug in jQuery 1.2.6 on KHTML
			var less = $( param.expanded ).css( 'display', 'none' );

			more.click( function( e ) {
				$( [ post, this, less ] ).toggle();
			} );

			less.click( function( e ) {
				$( [ post, more, this ] ).toggle();
			} );

			// for bug in jQuery 1.2.6 on KHTML
			$( this ).before( more ).after( less ).css( 'display', 'none' );
		} );
	},
	
	TagCloud: {
		
		cloudMin: 1,
		minFontSize: 10,
		maxFontSize: 20,
		minColor: [ 0xCC, 0xCC, 0xFF ],
		maxColor: [ 0x99, 0x99, 0xFF ],
		showCount: false,
		shuffle: false,

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
			var rsk = [];
			jQuery.each( tags, function( key, value ) {
				if( Blog.TagCloud.shuffle ) {
					rsk.splice( Math.floor( Math.random() * ( rsk.length + 1 ) ), 0, key );
				} else {
					rsk.push( key );
				}
				if( jQuery.inArray( value, c ) < 0 ) {
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
			
			jQuery.each( rsk, function( index, key ) {
				// 當標籤文章數小於最底限時, 跳過這次的迭代( 由於每次的迭代都是function, 用return取代continue )
				if( tags[key] < Blog.TagCloud.cloudMin ) {
					return true;
				}
				// 取得顏色
				jQuery.each( c, function( index ) {
					c[index] = helper( Blog.TagCloud.minColor[index], Blog.TagCloud.maxColor[index], tags[key] - ta, tz );
				} );
				// 建立清單項目
				var li = $( '<li/>' ).css( {
					// 取得字體大小
					fontSize: helper( Blog.TagCloud.minFontSize, Blog.TagCloud.maxFontSize, tags[key] - ta, tz ) + 'px',
					lineHeight: '1em'
				} ).append( $( '<a>' + key + '</a>' ).attr( {
					href: [ location.protocol, null, location.host, 'search', 'label', encodeURIComponent( key ) ].join( '/' ),
					title: tags[key] + ' Post' + ( tags[key] > 1 ? 's' : '' ) + ' in ' + key
				} ).css( {
					color: 'rgb( ' + c.join( ', ' ) + ' )'
				} ) );
				// 項目加入連結
				// 如果設定顯示篇數的話
				if( Blog.TagCloud.showCount ) {
					// 加入項目(在連結元素之後)
					li.append( '<span class="label-count">(' + tags[key] + ')</span>' );
				}
				// 清單加入項目
				ul.append( li ).append( ' ' );
				// 間隔字元(美化)
			} );

			// 加入清單
			lc2.append( ul );
		}

	},

	highlighting: function( cbs ) {
		SyntaxHighlighter.config.clipboardSwf = cbs;
		SyntaxHighlighter.config.bloggerMode = true;
		SyntaxHighlighter.all();
	},

	Media: {
		video: true,
		image: true,
		
		trigger: function() {
			// processing image
			$( 'img.Media' ).each( function() {
				var shell = $( '\
					<div style="overflow: auto; width: 100%;">\
						<a href="' + $( this ).attr( 'src' ) + '" rel="external"/>\
					</div>\
				' );
				$( this ).after( shell ).appendTo( shell.children( ':first' ) );
			} );

			// processing video
			$.fn.media.defaults.bgColor = '#000000';
			$( 'a.Media' ).click( function( e ) {
				if( Blog.Media.video && !e.ctrlKey ) {
					e.preventDefault();
					$( this ).unbind( 'click.target' ).media();
				}
			} );
			$( 'a.YouTube' ).click( function( e ) {
				if( Blog.Media.video && !e.ctrlKey ) {
					e.preventDefault();
					var temp = $( this ).attr( 'href' ).match( /http:\/\/tw\.youtube\.com\/watch\?(\w)=([\w-]+)/ );
					$( this ).unbind( 'click.target' ).media( {
						src: 'http://www.youtube.com/' + temp[1] + '/' + temp[2] + '&hl=zh_TW&fs=1',
						width: 425,
						height: 344,
						type: 'swf'
					} );
				}
			} );
			$( 'a.GoogleVideo' ).click( function( e ) {
				if( Blog.Media.video && !e.ctrlKey ) {
					e.preventDefault();
					var self = $( this );
					var url = self.attr( 'href' );
					var temp = url.match( /http:\/\/video\.google\.com\/videoplay\?(\w+)=(.+)/ );
					url = 'http://video.google.com/googleplayer.swf?' + temp[1] + '=' + temp[2] + '&fs=true';

					self.before( $( '<embed allowFullScreen="true" src="' + url + '" type="application/x-shockwave-flash" />' ).width( self.attr( 'width' ) ).height( self.attr( 'height' ) ) ).remove();
				}
			} );
		}

	}

};
