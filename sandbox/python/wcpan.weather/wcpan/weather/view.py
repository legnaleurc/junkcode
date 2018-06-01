from aiohttp import web as aw
import aiohttp_jinja2 as aj


class IndexHandler(aw.View):

    async def get(self):
        db = self.request.app['db']
        country_list = db.get_country_list()
        country_list = sorted(country_list, key=lambda _: _['name'])
        return aj.render_template('index.j2', self.request, {
            'country_list': country_list,
        })
