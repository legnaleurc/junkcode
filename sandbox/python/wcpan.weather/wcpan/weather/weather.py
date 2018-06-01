import aiohttp


API_HOST = 'api.openweathermap.org'


class Weather(object):

    def __init__(self, api_key):
        self._api_key = api_key
        self._session = aiohttp.ClientSession()

    async def __aenter__(self):
        await self._session.__aenter__()
        return self

    async def __aexit__(self, exc_type, exc, tb):
        await self._session.__aexit__(exc_type, exc, tb)

    async def get_weather_by_city_id(self, id_):
        url = self._create_weather_url(id_)
        async with self._session.get(url) as r:
            data = await r.json()
        return {
            'temp': data['main']['temp'],
            'temp_min': data['main']['temp_min'],
            'temp_max': data['main']['temp_max'],
            'icon': data['weather'][0]['id'],
        }

    def _create_weather_url(self, id_):
        return 'https://{0}/data/2.5/weather?id={1}&units=metric&appid={2}'.format(API_HOST, id_, self._api_key)
