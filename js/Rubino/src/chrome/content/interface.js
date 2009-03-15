var shell = {
	launcher: function() {
		window.openDialog( 'chrome://rubino/content/interface.xul', '', 'chrome,dialog=0,resizable=1,left=30,top=30,width=450,height=600' );
	},
	
	onLoad: function() {
		var cin = document.getElementById( 'stdin' );
		var mode = document.getElementById( 'modebox' );
		var HTMLcontent = {
			ruby: document.getElementById( 'out1' ).contentDocument.getElementsByTagName( 'pre' )[0],
			
			corundum: document.getElementById( 'out2' ).contentDocument.getElementsByTagName( 'pre' )[0],
			
			pure: document.getElementById( 'out3' ).contentDocument.getElementsByTagName( 'pre' )[0]
		};
		
		cin.inputField.onfocus = function() {
			this.select();
		};
		
		document.getElementById( 'exe' ).onclick = function() {
			HTMLcontent.ruby.innerHTML = cin.value.createTag( 'ja.Hiragana' );
			HTMLcontent.corundum.innerHTML = HTMLcontent.ruby.innerHTML.removeTag();
			HTMLcontent.pure.innerHTML = HTMLcontent.corundum.innerHTML.clearText( 'ja.Hiragana' );
		};
		
		document.getElementById( 'cls' ).onclick = function() {
			for( var content in HTMLcontent ) {
				HTMLcontent[content].innerHTML = '';
			}
		};
		
		document.getElementById( 'copy' ).onclick = function() {
			switch( document.getElementById( 'ostream' ).selectedIndex )
			{
			case 0:
				shell.copyToClipboard( HTMLcontent.ruby.innerHTML );
				break;
			case 1:
				shell.copyToClipboard( HTMLcontent.corundum.innerHTML );
				break;
			case 2:
				shell.copyToClipboard( HTMLcontent.pure.innerHTML );
				break;
			default:
				alert( 'undefined behavior!' );
			}
		};
	},
	
	copyToClipboard: function( S ) {
		var clipboard = Components.classes["@mozilla.org/widget/clipboardhelper;1"].getService(Components.interfaces.nsIClipboardHelper);
		clipboard.copyString( S );
	}
}