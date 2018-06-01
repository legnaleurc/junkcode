from aiohttp import web as aw


class CityHandler(aw.View):

    async def get(self):
        db = self.request.app['db']
        country_id = self.request.match_info.get('country_id')
        country_id = int(country_id)
        cities = db.get_city_list_by_country_id(country_id)
        return aw.json_response(cities)


class WeatherHandler(aw.View):

    async def get(self):
        db = self.request.app['db']
        w = self.request.app['weather']
        city_id = self.request.match_info.get('city_id')
        city_id = int(city_id)

        # first look up from cache, so we can save API quota
        weather_data = db.get_weather_by_city_id(city_id)
        if weather_data:
            return aw.json_response(weather_data)

        # if the cache is invalid, fetch new one
        weather_data = await w.get_weather_by_city_id(city_id)
        if weather_data:
            db.update_weather(city_id, weather_data)
            return aw.json_response(weather_data)

        # HACK quota exceed, return a random data
        return aw.json_response({
            'temp': 28,
            'temp_min': 26,
            'temp_max': 30,
            'icon': 300,
        })
