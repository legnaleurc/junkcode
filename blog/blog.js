// Depends on prototype.js

var Blog = {

	targetLink: function( id, dft, ct ) {
		var flag = true;
		var toggle = $( id );

		toggle.update( dft );

		toggle.observe( 'click', function( e ) {
			e.preventDefault();
			this.update( flag ? ct : dft );
			flag = !flag;
		} );

		$$( 'a[rel="external"]' ).each( function( link ) {
			link.observe( 'click', function( e ) {
				if( flag ) {
					e.preventDefault();
					window.open( this.readAttribute( 'href' ), '_blank' );
				}
			} );
		} );
	},

	hideBlocks: function( selector, collapsed, expanded ) {
		var posts = $$( selector );
		if( posts && collapsed && expanded ) {
			try {
				posts.each( function( post ) {
					var more = new Element( 'span', {
						'class': collapsed['class'],
						'title': collapsed['title']
					} );
					more.update( collapsed['textContent'] );

					var less = new Element( 'span', {
						'class': expanded['class'],
						'title': expanded['title']
					} );
					less.update( expanded['textContent'] );
					less.hide();

					more.observe( 'click', function( e ) {
						post.toggle();
						this.toggle();
						less.toggle();
					}.bindAsEventListener( more ) );

					less.observe( 'click', function( e ) {
						post.toggle();
						more.toggle();
						this.toggle();
					}.bindAsEventListener( less ) );

					post.insert( {
						'before': more,
						'after': less
					} );
					post.hide();
				} );
			} catch( e ) {
				$('stderr').update( e.message() );
			}
		}
	},
	
	TagCloud: {
		
		cloudMin: 1,
		minFontSize: 10,
		maxFontSize: 20,
		minColor: [ 0xCC, 0xCC, 0xFF ],
		maxColor: [ 0x99, 0x99, 0xFF ],
		showCount: false,
		shuffle: true,

		randomShuffle: function( hash ) {
			var result = new Hash();
			var keys = hash.keys();
			for( var i = 0, r, temp, len = keys.length; i < len; ++i ) {
				do {
					r = Math.floor( Math.random() * len );
				} while( r == i );
				temp = keys[i];
				keys[i] = keys[r];
				keys[r] = temp;
			}
			keys.each( function( key ) {
				result.set( key, hash.get( key ) );
			} );
			return result;
		},

		helper: function( a, b, i, x ) {
			if( a > b ) {
				return a - Math.floor( Math.log( i ) * ( a - b ) / Math.log( x ) );
			} else {
				return Math.floor( Math.log( i ) * ( b - a ) / Math.log( x ) + a );
			}
		},
		
		generate: function( tags ) {
			// 傳入的參數為Object, 轉換為Hash
			tags = $H( tags );
			tags.unset( '' );
			if( this.shuffle ) {
				tags = this.randomShuffle( tags );
			}
			// color的rgb
			var c = new Array( 3 );
			// 標籤的大小數量, 取得標籤的數量後做unique再取陣列長度
			var labelCount = tags.values().uniq().length;
			
			// 標籤數量底限
			var ta = this.cloudMin - 1;
			// 大小和顏色的總級數
			var tz = labelCount - this.cloudMin;
			// 放置標籤雲的元素
			var lc2 = $( 'labelCloud' );
			// 清單元素
			var ul = new Element( 'ul' );
			ul.addClassName( 'label-cloud' );
			
			tags.each( function( pair ) {
				// 當標籤文章數小於最底限時, 跳過這次的迭代( 由於每次的迭代都是function, 用return取代continue )
				if( pair.value < this.cloudMin ) {
					return;
				}
				// 取得顏色
				for( var i = 0; i < c.length; ++i ) {
					c[i] = this.helper( this.minColor[i], this.maxColor[i], pair.value - ta, tz );
				}
				// 取得字體大小
				var fs = this.helper( this.minFontSize, this.maxFontSize, pair.value - ta, tz );
				// 建立清單項目
				var li = new Element( 'li' );
				li.setStyle( {
					fontSize: fs + 'px',
					lineHeight: '1em'
				} );
				// 建立連結
				var a = new Element( 'a', {
					href: [ location.protocol, null, location.host, 'search', 'label', encodeURIComponent( pair.key ) ].join( '/' ),
					title: pair.value + ' Post' + ( pair.value > 1 ? 's' : '' ) + ' in ' + pair.key
				} );
				a.setStyle( {
					color: 'rgb( ' + c.join( ', ' ) + ' )'
				} );
				a.appendChild( document.createTextNode( pair.key ) );
				// 項目加入連結
				li.appendChild( a );
				// 如果設定顯示篇數的話
				if( this.showCount ) {
					// 建立行內型元素
					var span = new Element( 'span' );
					span.addClassName( 'label-count' );
					span.appendChild( document.createTextNode( '(' + pair.value + ')' ) );
					// 加入項目(在連結元素之後)
					li.appendChild( span );
				}
				// 清單加入項目
				ul.appendChild( li );
				// 間隔字元(美化)
				ul.appendChild( document.createTextNode( ' ' ) );
			}.bind( this ) );

			// 加入清單
			lc2.appendChild( ul );
		}

	},

	highlighting: function() {
		dp.SyntaxHighlighter.ClipboardSwf = 'http://career.ccu.edu.tw/~legnaleurc/library/dp.SyntaxHighlighter/Scripts/clipboard.swf';
		dp.SyntaxHighlighter.BloggerMode();
		dp.SyntaxHighlighter.HighlightAll( 'code' );
	},

	Media: {
		trigger: function() {
			var temp = $$( 'a.Media' );
			if( temp ) {
				temp.each( function( X ) {
					X.observe( 'click', Blog.Media.handler.bindAsEventListener( this ) );
				}.bind( this ) );
			}
			temp = $$( 'a.YouTube' );
			if( temp ) {
				temp.each( function( X ) {
					X.observe( 'click', Blog.Media.youtube.bindAsEventListener( this ) );
				}.bind( this ) );
			}
			temp = $$( 'a.GoogleVideo' );
			if( temp ) {
				temp.each( function( X ) {
					X.observe( 'click', Blog.Media.googlevideo.bindAsEventListener( this ) );
				}.bind( this ) );
			}
		},
		
		youtube: function( e ) {
			e.preventDefault();
			var E = Event.element( e );
			var url = E.readAttribute( 'href' );
			var temp = url.match( /http:\/\/tw\.youtube\.com\/watch\?(\w)=(\w+)/ );
			url = 'http://www.youtube.com/' + temp[1] + '/' + temp[2] + '&hl=zh_TW&fs=1';
			
			var node = new Element( 'object', { width: '425', height: '344' } );
			node.appendChild( new Element( 'param', { name: 'movie', value: url } ) );
			node.appendChild( new Element( 'param', { name: 'allowFullScreen', value: 'true' } ) );
			node.appendChild( new Element( 'embed', { src: url, type: 'application/x-shockwave-flash', allowfullscreen: 'true', width: '425', height: '344' } ) );
			E.parentNode.insertBefore( node, E );
			E.stopObserving( 'click', this.youtube );
			E.remove();
		},
		
		googlevideo: function( e ) {
			e.preventDefault();
			var E = Event.element( e );
			var url = E.readAttribute( 'href' );
			var dim = { width: E.readAttribute( 'width' ) + 'px', height: E.readAttribute( 'height' ) + 'px' };
			var temp = url.match( /http:\/\/video\.google\.com\/videoplay\?(\w+)=(.+)/ );
			url = 'http://video.google.com/googleplayer.swf?' + temp[1] + '=' + temp[2] + '&fs=true';
			
			var node = new Element( 'embed', { allowFullScreen: 'true', src: url, type: 'application/x-shockwave-flash' } );
			node.setStyle( dim );
			E.parentNode.insertBefore( node, E );
			E.stopObserving( 'click', this.googlevideo );
			E.remove();
		},

		handler: function( e ) {
			e.preventDefault();
			var E = Event.element( e );
			var attr = {
				value: E.readAttribute( 'href' ),
				width: E.readAttribute( 'width' ),
				height: E.readAttribute( 'height' ),
				uiMode: E.readAttribute( 'uimode' ),
				autoStart: E.readAttribute( 'autostart' )
			};
			attr.type = this.getMIME( attr.value );
			if( !isNaN( attr.width ) )
				attr.width += 'px';
			if( !isNaN( attr.height ) )
				attr.height += 'px';
///*
			var media = new Element( 'object', { data: attr.value, type: attr.type } );
			media.setStyle( {
				width: attr.width,
				height: attr.height
			} );
			media.appendChild( new Element( 'param', { name: 'FileName', value: attr.value } ) );
			media.appendChild( new Element( 'param', { name: 'src', value: attr.value } ) );
			media.appendChild( new Element( 'param', { name: 'Scale', value: 'Aspect' } ) );
			if( attr.uiMode ) {
				media.appendChild( new Element( 'param', { name: 'uiMode', value: attr.uiMode } ) );
			}
			if( attr.autoStart ) {
				media.appendChild( new Element( 'param', { name: 'AutoPlay', value: attr.autoStart } ) );
				media.appendChild( new Element( 'param', { name: 'AutoStart', value: attr.autoStart } ) );
			}
			E.parentNode.insertBefore( media, E );
			E.stopObserving( 'click', this.handler );
			E.remove();
//*/
/*
			var str = '<object data="' + attr.value + '" type="' + attr.type + '" style="width: ' + attr.width + '; height: ' + attr.height + ';">';
			str += '<param name="FileName" value="' + attr.value + '" />';
			str += '<param name="src" value="' + attr.value + '" />';
			str += '<param name="Scale" value="Aspect" />';
			if( attr.uiMode ) {
				str += '<param name="uiMode" value="' + attr.uiMode + '" />';
			}
			if( attr.autoStart ) {
				str += '<param name="AutoPlay" value="' + attr.autoStart + '" />';
				str += '<param name="AutoStart" value="' + attr.autoStart + '" />';
			}
			str += '</object>';

			E.stopObserving( 'click', Blog.Media.handler );
			E.replace( str );
*/
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
