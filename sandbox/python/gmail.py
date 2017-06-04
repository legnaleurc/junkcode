#! /usr/bin/env python


import logging
import logging.handlers


EMAIL_HOST_PASSWORD = ''
EMAIL_HOST_USER = ''
GOOGLE_API_CLIENT_ID = ''
GOOGLE_API_CLIENT_SECRET = ''


class GMailHandler(logging.handlers.SMTPHandler):

	def __init__(self, mailhost, fromaddr, toaddrs, subject, username, timeout=1.0):
		super(GMailHandler, self).__init__(mailhost, fromaddr, toaddrs, subject, credentials=None, secure=None, timeout=timeout)

		self.username = username

	def emit(self, record):
		try:
			import smtplib
			from email.utils import formatdate
			port = self.mailport
			if not port:
				port = smtplib.SMTP_PORT
			smtp = smtplib.SMTP(self.mailhost, port, timeout=self.timeout)
			msg = self.format(record)
			msg = (
				'From: {0}\r\n'
				'To: {1}\r\n'
				'Subject: {2}\r\n'
				'Date: {3}\r\n'
				'\r\n'
				'{4}'
			).format(
				self.formaddr,
				','.join(self.toaddrs),
				self.getSubject(record),
				formatdate(),
				msg,
			)
			if self.username:
				if self.secure is not None:
					smtp.ehlo()
					smtp.starttls(*self.secure)
					smtp.ehlo()
				smtp.login(self.username, self.password)
			smtp.sendmail(self.fromaddr, self.toaddrs, msg)
			smtp.quit()
		except Exception:
			self.handleError(record)

	def _open(self):
		if self._connection:
			return False
		try:
			gauth = urllib.urlopen(self._request_url, urllib.urlencode(self._param))
			response = gauth.read()
			gauth.close()
			response = json.loads(response)
			access_token = response['access_token']
			access_token = 'user={0}\1auth=Bearer {1}\1\1'.format(self.username, access_token)
			;


a = GMailHandler('smtp.gmail.com', EMAIL_HOST_USER, EMAIL_HOST_USER, '__TEST__')

print(dir(a))
