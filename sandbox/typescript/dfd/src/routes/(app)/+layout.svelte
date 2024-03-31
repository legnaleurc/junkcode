<script lang="ts">
  import { QueryClientProvider } from "@tanstack/svelte-query";

  import type { LayoutData } from "./$types";

  import "../../app.css";

  export let data: LayoutData;
</script>

<QueryClientProvider client={data.queryClient}>
  <slot />
</QueryClientProvider>
