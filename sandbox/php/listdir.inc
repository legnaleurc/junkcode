<?php
class ListDir {

	public function __construct( $id, $class, $dirClass = 'titles' ) {
		$this->id_ = $id;
		$this->class_ = $class;
		$this->dirClass = $dirClass;
		$this->path_ = '.';
		$this->depth_ = 0;
		$this->ignore_ = '(^\..*|(.+\.php$)|cgi-bin)';
	}

	public function setIgnorePattern( $ignore ) {
		if( !ereg( $ignore, '.' ) || !ereg( $ignore, '..' ) ) {
			$ignore = '^\.{1,2}$|' . $ignore;
		}
		$this->ignore_ = $ignore;
	}

	/**
	 * @brief Traverse directories and files.
	 */
	public function perform() {
		echo "<!-- listing directories BEGIN -->\n";
		printf( '<div id="%s" class="%s">'."\n", $this->id_, $this->class_ );

		$this->traverse( $this->path_, 0 );

		echo "</div>\n";
		echo "<!-- listing directories END -->\n";
	}

	private function traverse( $path, $level ) {
		if( $this->depth_ == 0 || $level < $this->depth_ ) {
			$contents = scandir( $path );
			if( !$contents ) {
				return;
			}

			printf( '<ul class="%s">'."\n", $this->class_ );

			foreach( $contents as $entry ) {
				if( ereg( $this->ignore_, $entry ) ) {
					continue;
				}

				printf( '<li class="%s">'."\n", $this->class_ );

				if( is_dir( "$path/$entry" ) ) {
					printf( '<strong class="%s">%s</strong>'."\n", ( ( $level == 0 ) ? "$this->dirClass_ $this->class_" : '' ), $entry );
					$this->traverse( "$path/$entry", $level + 1 );
				} else {
					printf( '<a title="%s" href="%s">%s</a>'."\n", self::getSize( "$path/$entry" ), "$path/$entry", $entry );
				}

				echo "</li>\n";
			}

			echo "</ul>\n";
		}
	}

	private static function getSize( $filepath ) {
		$size = filesize( $filepath );
		$counter = 0;

		while( $size >= 1024 )
		{
			$size /= 1024;
			$counter++;
		}

		switch( $counter )
		{
			case 0:
				return "$size Bytes";
			case 1:
				return "$size KB";
			case 2:
				return "$size MB";
			case 3:
				return "$size GB";
			default:
				return 'Unknow size.';
		}
	}

	private $id_;
	private $class_;
	private $dirClass_;
	private $path_;
	private $depth_;
	private $ignore_;

}
?>
