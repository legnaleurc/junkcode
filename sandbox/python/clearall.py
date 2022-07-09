#! /usr/bin/env python3

from urllib.parse import parse_qsl
import time

from requests_oauthlib import OAuth1Session
from TwitterAPI import TwitterAPI


BASE_URL = 'https://api.twitter.com/{0}'
CALLBACK_URL = 'http://127.0.0.1:8000/api/v1/authorize'
API_KEY = ''
API_SECRET_KEY = ''
ACCESS_TOKEN = ''
ACCESS_TOKEN_SECRET = ''


def main():
    twitter = TwitterAPI(API_KEY, API_SECRET_KEY, ACCESS_TOKEN, ACCESS_TOKEN_SECRET)
    while True:
        try:
            delete_tweets(twitter)
            time.sleep(60)
        except Exception:
            print('stop')
            break
    return 0


def delete_tweets(twitter: TwitterAPI):
    timeline = twitter.request('statuses/user_timeline', {
        'count': 200,
    })
    if timeline.status_code != 200:
        raise Exception('expired?')
    for tweet in timeline:
        if 'id' in tweet:
            tweet_id = tweet['id']
            rv = twitter.request(f'statuses/destroy/:{tweet_id}')
            print(rv.text)
            if rv.status_code != 200:
                raise Exception('should stop')


def get_limits(twitter: TwitterAPI):
    limits = twitter.request('application/rate_limit_status', {
        'resource': 'statuses',
    })
    print(limits.json())


def get_access_token():
    url = BASE_URL.format('oauth/request_token')
    twitter = OAuth1Session(API_KEY, API_SECRET_KEY)
    rv = twitter.post(url, params={
        'oauth_callback': 'oob',
    })
    credentials = dict(parse_qsl(rv.text))
    request_key = credentials.get('oauth_token')
    request_secret = credentials.get('oauth_token_secret')

    print(f'Go here to authorize:\n  https://api.twitter.com/oauth/authorize?oauth_token={request_key}')
    verifier = input()

    url = BASE_URL.format('oauth/access_token')
    twitter = OAuth1Session(API_KEY, API_SECRET_KEY, request_key, request_secret, verifier=verifier)
    rv = twitter.post(url)
    credentials = dict(parse_qsl(rv.text))
    access_token_key = credentials.get('oauth_token')
    access_token_secret = credentials.get('oauth_token_secret')

    return access_token_key, access_token_secret


if __name__ == '__main__':
    main()
