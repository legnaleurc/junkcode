function Rubytext( string ) {
	var self = string;

	this.toString = function() {
		return this.valueOf();
	};

	this.valueOf = function() {
		if( self && typeof( self ) === 'string' ) {
			return self + '';
		} else {
			throw new TypeError();
		}
	};

	this.createTag = function( filter ) {
		self = self.replace( eval( 'Rubytext.Filter.' + filter ), '<ruby><rb>$1</rb><rp>$2</rp><rt>$3</rt><rp>$4</rp></ruby>' );
		return this;
	};

	this.removeTag =  function() {
		self = self.replace( Rubytext.Filter.Utility.RubyTag, '' );
		return this;
	};

	this.clearText = function( filter ) {
		self = self.replace( eval( 'Rubytext.Filter.' + filter ), '$1' );
		return this;
	};

	this.clearBase = function( filter ) {
		self = self.replace( eval( 'Rubytext.Filter.' + filter ), '$3' );
		return this;
	};

	this.rubyTransform = function() {
		self = self.replace( Rubytext.Filter.ja.HGYouon, function( m0, offset ) {
			return Rubytext.Roman.ja.Hiragana[m0];
		} ).replace( Rubytext.Filter.ja.HGSingle, function( m0, offset ) {
			return Rubytext.Roman.ja.Hiragana[m0]
		} ).replace( Rubytext.Filter.ja.HGSokuon, function( m0, offset, s ) {
			return s[offset+1];
		} );
		return this;
	};
}

Rubytext.prototype = new String;

Rubytext.version = '0.1.0';

Rubytext.loadCSS = function( uri ) {
	if( navigator.userAgent.search( /Opera|MSIE/ ) < 0 ) {
		var _head = document.getElementsByTagName( 'head' );
		var _link = document.createElement( 'link' );
		_link.rel = 'stylesheet';
		_link.type = 'text/css';
		_link.href = uri;
		_head[0].appendChild( _link );
	}
};

Rubytext.Filter = {
	Utility: {
		// Grab ruby tag.
		RubyTag: /<\/?r(uby|p|([bt]c?))>/g
	},

	ja: {
		// [\u4E00-\u9FFF] is CJK Unified Ideographs, [\u3040-\u309F] is Hiragana.
		Hiragana: /([\u4E00-\u9FFF]+)(\()([\u3040-\u309F]+)(\))/g,
		HGYouon: /[\u304D\u304E\u3057\u3058\u3061\u3062\u306B\u3072\u3073\u3074\u307F\u3089][\u3083\u3085\u3087]/g,
		HGSokuon: /\u3063/g,
		HGSingle: /[\u3042\u3044\u3046\u3048\u304A\u304B-\u3062\u3064-\u3082\u3084\u3086\u3088\u3089-\u308D\u308F-\u3093]/g
	}
};

Rubytext.Roman = {
	ja: {
		Hiragana: {
			'\u3042': 'a',  '\u3044': 'i',   '\u3046': 'u',   '\u3048': 'e',  '\u304A': 'o',
			'\u304B': 'ka', '\u304D': 'ki',  '\u304F': 'ku',  '\u3051': 'ke', '\u3053': 'ko',
			'\u3055': 'sa', '\u3057': 'shi', '\u3059': 'su',  '\u305B': 'se', '\u305D': 'so',
			'\u305F': 'ta', '\u3061': 'chi', '\u3064': 'tsu', '\u3066': 'te', '\u3068': 'to',
			'\u306A': 'na', '\u306B': 'ni',  '\u306C': 'nu',  '\u306D': 'ne', '\u306E': 'no',
			'\u306F': 'ha', '\u3072': 'hi',  '\u3075': 'hu',  '\u3078': 'he', '\u307B': 'ho',
			'\u307E': 'ma', '\u307F': 'mi',  '\u3080': 'mu',  '\u3081': 'me', '\u3082': 'mo',
			'\u3084': 'ya',                  '\u3086': 'yu',                  '\u3088': 'yo',
			'\u3089': 'ra', '\u308A': 'ri',  '\u308B': 'ru',  '\u308C': 're', '\u308D': 'ro',
			'\u308F': 'wa', '\u3090': 'wi',                   '\u3091': 'we', '\u3092': 'wo',
			'\u3093': 'n',

			'\u304C': 'ga', '\u304E': 'gi', '\u3050': 'gu', '\u3052': 'ge', '\u3054': 'go',
			'\u3056': 'za', '\u3058': 'ji', '\u305A': 'zu', '\u305C': 'ze', '\u305E': 'zo',
			'\u3060': 'da', '\u3062': 'di', '\u3065': 'du', '\u3067': 'de', '\u3069': 'do',
			'\u3070': 'ba', '\u3073': 'bi', '\u3076': 'bu', '\u3079': 'be', '\u307C': 'bo',

			'\u3071': 'pa', '\u3074': 'pi', '\u3077': 'pu', '\u307A': 'pe', '\u307D': 'po',

			'\u304D\u3083': 'kya', '\u304D\u3085': 'kyu', '\u304D\u3087': 'kyo', '\u304E\u3083': 'gya', '\u304E\u3085': 'gyu', '\u304E\u3087': 'gyo',
			'\u3057\u3083': 'sya', '\u3057\u3085': 'syu', '\u3057\u3087': 'syo', '\u3058\u3083': 'ja', '\u3058\u3085': 'ju', '\u3058\u3087': 'jo',
			'\u3061\u3083': 'cha', '\u3061\u3085': 'chu', '\u3061\u3087': 'cho', '\u3062\u3083': 'zya', '\u3062\u3085': 'zyu', '\u3062\u3087': 'zyo',
			'\u306B\u3083': 'nya', '\u306B\u3085': 'nyu', '\u306B\u3087': 'nyo',
			'\u3072\u3083': 'hya', '\u3072\u3085': 'hyu', '\u3072\u3087': 'hyo', '\u3073\u3083': 'bya', '\u3073\u3085': 'byu', '\u3073\u3087': 'byo', '\u3074\u3083': 'pya', '\u3074\u3085': 'pyu', '\u3074\u3087': 'pyo',
			'\u307F\u3083': 'mya', '\u307F\u3085': 'myu', '\u307F\u3087': 'myo',
			'\u3089\u3083': 'rya', '\u3089\u3085': 'ryu', '\u3089\u3087': 'ryo'
		}
	}
};
