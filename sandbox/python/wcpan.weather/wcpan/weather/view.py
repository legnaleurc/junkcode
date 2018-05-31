from aiohttp import web as aw
import aiohttp_jinja2 as aj


class IndexHandler(aw.View):

    async def get(self):
        return aj.render_template('index.html', self.request, None)
