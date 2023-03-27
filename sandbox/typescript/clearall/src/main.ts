import { Client } from "twitter-api-sdk";
import { parseISO, differenceInMilliseconds } from "date-fns";

import { createAuthUser } from "./lib/auth";
import {
  blockUserId,
  getMyId,
  iterListsIncludesUserId,
  iterTweetList,
  iterUserFollowing,
} from "./lib/tweet";
import { INTERVAL_MS } from "./lib/consts";

async function main() {
  const authUser = await createAuthUser();

  const client = new Client(authUser);
  const userId = await getMyId(client);
  await clearRecentTweets(client, userId);
  await clearUnknownLists(client, userId);
}

async function clearRecentTweets(client: Client, userId: string) {
  const now = new Date();
  for await (const tweet of iterTweetList(client, userId)) {
    const date = parseISO(tweet.created_at!);
    if (differenceInMilliseconds(now, date) < INTERVAL_MS) {
      console.debug("skipped", tweet.text);
      continue;
    }
    await client.tweets.deleteTweetById(tweet.id);
    console.debug("deleted", tweet.text);
  }
}

async function clearUnknownLists(client: Client, userId: string) {
  const following = new Set<string>();
  for await (const f of iterUserFollowing(client, userId)) {
    following.add(f.id);
  }
  type List = {
    id: string;
    name: string;
    description?: string;
    owner_id: string;
  };
  const black = new Map<string, List[]>();
  for await (const l of iterListsIncludesUserId(client, userId)) {
    if (l.private) {
      continue;
    }
    if (!l.owner_id) {
      continue;
    }
    if (following.has(l.owner_id)) {
      continue;
    }
    appendToMap(black, l.owner_id, {
      id: l.id,
      name: l.name,
      description: l.description,
      owner_id: l.owner_id,
    });
  }
  for (const [id, listList] of black.entries()) {
    for (const list of listList) {
      console.debug(`found ${list.name}`);
      if (list.description) {
        console.debug(`which ${list.description}`);
      }
    }
    const ok = await blockUserId(client, userId, id);
    console.debug(`block ${id} ${ok}`);
  }
}

function appendToMap<K, V>(map: Map<K, V[]>, key: K, value: V): void {
  if (map.has(key)) {
    const list = map.get(key) ?? [];
    list.push(value);
  } else {
    map.set(key, [value]);
  }
}

main().catch((e: unknown) => {
  console.error(e);
});
