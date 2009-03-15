var RubyText = {
	Version: '2.4.0',
	
	loadCSS: function( S ) {
		if( navigator.userAgent.search( /Opera|MSIE/ ) < 0 ) {
			var _head = document.getElementsByTagName( 'head' );
			var _link = document.createElement( 'link' );
			_link.rel = 'stylesheet';
			_link.type = 'text/css';
			_link.href = S;
			_head[0].appendChild( _link );
		}
	},
	
	Extention: {
		createTag: function( S ) {
			return eval( "this.replace( RubyText.Filter." + S + " , '<ruby><rb>$1</rb><rp>$2</rp><rt>$3</rt><rp>$4</rp></ruby>' )" );
		},
		
		removeTag: function() {
			return this.replace( RubyText.Filter.Utility.RubyTag, '' );
		},
		
		clearText: function( S ) {
			return eval( "this.replace( RubyText.Filter." + S + ", '$1' )" );
		}
	},
	
	Filter: {
		Utility: {
			// Grab ruby tag.
			RubyTag: /<\/?r(uby|p|([bt]c?))>/g
		},
		
		ja: {
			// [\u4E00-\u9FFF] is CJK Unified Ideographs, [\u3040-\u309F] is Hiragana.
			Hiragana: /([\u4E00-\u9FFF]+)(\()([\u3040-\u309F]+)(\))/g
		}
	}
};

for( var property in RubyText.Extention )
	String.prototype[property] = RubyText.Extention[property];
