from aiohttp import web as aw


class CityHandler(aw.View):

    async def get(self):
        db = self.request.app['db']
        country_id = self.request.match_info.get('country_id')
        country_id = int(country_id)
        cities = db.get_city_list_from_country_id(country_id)
        return aw.json_response(cities)
