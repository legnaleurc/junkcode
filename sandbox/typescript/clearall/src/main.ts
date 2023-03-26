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
  const black = new Set<string>();
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
    console.debug(`block ${l.name}`);
    black.add(l.owner_id);
  }
  for (const id of black.values()) {
    const ok = await blockUserId(client, userId, id);
    console.debug(`block ${userId} ${ok}`);
  }
}

main().catch((e: unknown) => {
  console.error(e);
});
