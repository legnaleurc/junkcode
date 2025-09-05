import asyncio
import sys

from gql import Client, gql
from gql.transport.aiohttp import AIOHTTPTransport


_API_URL = "https://api.tv.dmm.co.jp/graphql"


def load_query(query_name: str) -> str:
    with open(f"queries/{query_name}.gql", "r") as f:
        return f.read()


async def main(args: list[str] | None = None) -> int:
    if not args:
        return 1

    query_name = args[1]
    id = args[2]

    transport = AIOHTTPTransport(url=_API_URL)

    client = Client(transport=transport, fetch_schema_from_transport=False)

    _QUERY = load_query(query_name)
    query = gql(_QUERY)

    async with client as session:
        result = await session.execute(
            query,
            variable_values={
                "id": id,
                "device": "BROWSER",
                "isForeign": False,
            },
        )

    print(result)
    return 0


if __name__ == "__main__":
    sys.exit(asyncio.run(main(sys.argv)))
