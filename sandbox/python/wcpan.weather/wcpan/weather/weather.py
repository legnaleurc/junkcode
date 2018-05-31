import aiohttp


API_HOST = 'samples.openweathermap.org'


class Weather(object):

    def __init__(self, api_key):
        self._api_key = api_key
        self._session = aiohttp.ClientSession()

    async def __aenter__(self):
        await self._session.__aenter__()

    async def __aexit__(self, exc_type, exc, tb):
        await self._session.__aexit__(exc_type, exc, tb)

    async def get_weather_from_city_id(self, id_):
        url = self._create_forecast_url(id_)
        async with self._session.get(url) as r:
            data = await r.json()
        return data

    def _create_forecast_url(self, id_):
        return 'https://{0}/data/2.5/forecast?id={1}&appid={2}'.format(API_HOST, id_, self._api_key)
