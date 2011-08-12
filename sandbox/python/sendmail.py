#! /usr/bin/env python
#-*- coding: utf-8 -*-

from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.application import MIMEApplication
from email.Utils import COMMASPACE, formatdate
import smtplib

def sign( content, email, passphrase ):
	import GnuPGInterface

	gnupg = GnuPGInterface.GnuPG()
	gnupg.options.armor = 1
	gnupg.options.meta_interactive = 0

	p = gnupg.run( [ '-bu' ], [ email ], create_fhs = [ 'stdin', 'stdout', 'passphrase' ] )

	p.handles['passphrase'].write( passphrase )
	p.handles['passphrase'].close()

	p.handles['stdin'].write( content )
	p.handles['stdin'].close()

	signed = p.handles['stdout'].read()
	p.handles['stdout'].close()

	p.wait()
	return signed

src = 'legnaleurc@gmail.com'
dst = [ 'legnaleurc@ubuntu-tw.org' ]
subject = '__TEST__'
content = 'Hello, world!'

msg = MIMEMultipart( 'signed', protocol = 'application/pgp-signature', micalg = 'pgp-sha1' )
msg['From'] = src
msg['To'] = COMMASPACE.join( dst )
msg['Date'] = formatdate( localtime = True )
msg['Subject'] = subject

body = MIMEText( content )
body.set_charset( 'utf-8' )

msg.attach( body )

def helper( mime ):
	signed = sign( mime.get_payload(), src, '__GPG_PASSWORD__' )
	mime.set_payload( signed )
	mime.add_header( 'Content-Description', 'This is a digitally signed message part.' )

part = MIMEApplication( content, 'pgp-signature', helper, name = 'signature.asc' )
msg.attach( part )

sender = smtplib.SMTP_SSL( 'smtp.googlemail.com', 465 )
sender.login( src, '__MAIL_PASSWORD__' )
sender.sendmail( src, dst, msg.as_string() )
sender.quit()
