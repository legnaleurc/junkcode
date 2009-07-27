<?php
class ListDir {

	public function __construct( $id, $class, $dir_class = 'titles' ) {
		$this->id = $id;
		$this->class = $class;
		$this->dir_class = $dir_class;
	}

	/**
	 * @brief Traverse directories and files.
	 * @param $path Start point of traversal.
	 * @param $limit Traverse directory level.
	 * @param $ignore Regular expression of ignore file.
	 */
	public function perform( $path = '.', $limit = 0, $ignore = '(^\..*|(.+\.php$)|cgi-bin)' ) {
		printf( '<div id="%s" class="%s">'."\n", $this->id, $this->class );
		$this->traverse( $path, $limit, $ignore );
		echo "</div>\n";
	}

	private function traverse( $path, $limit, $ignore ) {
		static $level = 0;
		if( $limit == 0 || $level < $limit ) {
			$indent = str_repeat( "\t", $level + 1 );
			$dirHandler = opendir( $path );

			printf( '%s<ul class="%s">'."\n", "$indent", $this->class );

			if( !ereg( $ignore, '.' ) || !ereg( $ignore, '..' ) )
				$ignore = '^\.{1,2}$|' . $ignore;
			while( false !== ( $file = readdir( $dirHandler ) ) ) {
				if( !ereg( $ignore, $file ) ) {
					printf( '%s<li class="%s">'."\n", "$indent", $this->class );

					if( is_dir( "$path/$file" ) ) {
						printf( '%s<strong class="%s">%s</strong>'."\n", "$indent", ( ( $level == 0 ) ? "$this->dir_class $this->class" : '' ), $file );
						$level++;
						$this->traverse( "$path/$file", $limit, $ignore );
						$level--;
					} else {
						printf( '%s<a title="%s" href="%s">%s</a>'."\n", "$indent", self::getSize( "$path/$file" ), "$path/$file", $file );
					}

					echo "$indent</li>\n";
				}
			}

			echo "$indent</ul>\n";

			closedir( $dirHandler );
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

	private $id;

	private $class;

	private $dir_class;

}
?>
