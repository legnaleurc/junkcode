#!/usr/bin/env python3
"""
Fix file and folder names to be valid across Linux, MacOS, and Windows.

This script walks a directory tree and renames files/folders with invalid characters:
- Visible invalid characters are replaced with full-width equivalents
- Invisible control characters are stripped

Designed to run on Linux systems to prepare files for cross-platform sharing.
"""

import os
import sys
import unicodedata
from pathlib import Path


# Invalid characters for Windows (most restrictive)
INVALID_CHARS = {
    '<': '\uff1c',  # FULLWIDTH LESS-THAN SIGN
    '>': '\uff1e',  # FULLWIDTH GREATER-THAN SIGN
    ':': '\uff1a',  # FULLWIDTH COLON
    '"': '\uff02',  # FULLWIDTH QUOTATION MARK
    '/': '\uff0f',  # FULLWIDTH SOLIDUS
    '\\': '\uff3c', # FULLWIDTH REVERSE SOLIDUS
    '|': '\uff5c',  # FULLWIDTH VERTICAL LINE
    '?': '\uff1f',  # FULLWIDTH QUESTION MARK
    '*': '\uff0a',  # FULLWIDTH ASTERISK
}

# Reserved names on Windows
RESERVED_NAMES = {
    'CON', 'PRN', 'AUX', 'NUL',
    'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6', 'COM7', 'COM8', 'COM9',
    'LPT1', 'LPT2', 'LPT3', 'LPT4', 'LPT5', 'LPT6', 'LPT7', 'LPT8', 'LPT9',
}


def is_control_char(char):
    """Check if character is a control character (invisible)."""
    category = unicodedata.category(char)
    # Control characters: Cc (control), Cf (format), Cs (surrogate), Co (private use)
    return category in ('Cc', 'Cf', 'Cs', 'Co') or ord(char) < 32


def sanitize_name(name):
    """
    Sanitize a filename to be valid across Linux, MacOS, and Windows.

    Args:
        name: Original filename

    Returns:
        Sanitized filename
    """
    if not name:
        return name

    result = []
    for char in name:
        if char in INVALID_CHARS:
            # Replace visible invalid chars with full-width version
            result.append(INVALID_CHARS[char])
        elif is_control_char(char):
            # Strip control characters (invisible/harmful)
            continue
        else:
            result.append(char)

    sanitized = ''.join(result)

    # Remove trailing dots and spaces (problematic on Windows)
    sanitized = sanitized.rstrip('. ')

    # If name becomes empty after sanitization, use a default
    if not sanitized:
        sanitized = 'unnamed'

    # Check for reserved names (Windows)
    name_upper = sanitized.upper()
    # Check both exact match and with extension (e.g., CON.txt)
    base_name = name_upper.split('.')[0] if '.' in name_upper else name_upper
    if base_name in RESERVED_NAMES:
        sanitized = f'_{sanitized}'

    return sanitized


def get_unique_name(directory, name):
    """
    Get a unique name in the directory if the name already exists.

    Args:
        directory: Parent directory path
        name: Desired name

    Returns:
        Unique name that doesn't exist in the directory
    """
    path = directory / name
    if not path.exists():
        return name

    # Split name into base and extension
    if '.' in name and not name.startswith('.'):
        base, ext = name.rsplit('.', 1)
        ext = f'.{ext}'
    else:
        base = name
        ext = ''

    # Try adding numbers until we find a unique name
    counter = 1
    while True:
        new_name = f'{base}_{counter}{ext}'
        if not (directory / new_name).exists():
            return new_name
        counter += 1


def fix_names(root_path):
    """
    Walk the directory tree and fix invalid file/folder names.

    Args:
        root_path: Root directory to process
    """
    root_path = Path(root_path).resolve()

    if not root_path.exists():
        print(f'Error: Path does not exist: {root_path}')
        sys.exit(1)

    if not root_path.is_dir():
        print(f'Error: Path is not a directory: {root_path}')
        sys.exit(1)

    print(f'Scanning: {root_path}')
    print()

    # Walk bottom-up so we rename children before parents
    for dirpath, dirnames, filenames in os.walk(root_path, topdown=False):
        current_dir = Path(dirpath)

        # Fix filenames
        for filename in filenames:
            sanitized = sanitize_name(filename)
            if sanitized != filename:
                old_path = current_dir / filename
                new_name = get_unique_name(current_dir, sanitized)
                new_path = current_dir / new_name

                try:
                    old_path.rename(new_path)
                    print(f'Renamed file: {old_path}')
                    print(f'         to : {new_path}')
                    print()
                except Exception as e:
                    print(f'Error renaming {old_path}: {e}')
                    print()

        # Fix directory names
        for dirname in dirnames:
            sanitized = sanitize_name(dirname)
            if sanitized != dirname:
                old_path = current_dir / dirname
                new_name = get_unique_name(current_dir, sanitized)
                new_path = current_dir / new_name

                try:
                    old_path.rename(new_path)
                    print(f'Renamed dir : {old_path}')
                    print(f'         to : {new_path}')
                    print()
                except Exception as e:
                    print(f'Error renaming {old_path}: {e}')
                    print()

    print('Done!')


def main():
    """Main entry point."""
    if len(sys.argv) != 2:
        print('Usage: fix_name_for_fs.py <path>')
        print()
        print('Fix file and folder names to be valid across Linux, MacOS, and Windows.')
        print('- Visible invalid characters are replaced with full-width equivalents')
        print('- Invisible control characters are stripped')
        sys.exit(1)

    path = sys.argv[1]
    fix_names(path)


if __name__ == '__main__':
    main()
