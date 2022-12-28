import { Client } from "twitter-api-sdk";
import { parseISO, differenceInMilliseconds } from "date-fns";

import { createAuthUser } from "./lib/auth";
import { getMyId, iterTweetList } from "./lib/tweet";
import { INTERVAL_MS } from "./lib/consts";

async function main() {
  const authUser = await createAuthUser();

  const now = new Date();
  const client = new Client(authUser);
  const userId = await getMyId(client);
  for await (const tweet of iterTweetList(client, userId)) {
    const date = parseISO(tweet.created_at!);
    if (differenceInMilliseconds(now, date) < INTERVAL_MS) {
      console.info("skipped", tweet.text);
      continue;
    }
    await client.tweets.deleteTweetById(tweet.id);
    console.info("deleted", tweet.text);
  }
}

main();
