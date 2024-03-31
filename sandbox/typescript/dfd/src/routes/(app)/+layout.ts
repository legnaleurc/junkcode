import { browser } from "$app/environment";
import { QueryClient } from "@tanstack/svelte-query";

import type { LayoutLoad } from "./$types";

export const load = (async () => {
  const queryClient = new QueryClient({
    defaultOptions: {
      queries: {
        enabled: browser,
      },
    },
  });

  return { queryClient };
}) satisfies LayoutLoad;
