<script lang="ts">
  import { createMutation, useQueryClient } from "@tanstack/svelte-query";

  import { postFilter, type MutableFilter } from "$lib/api/filters";
  import { enterdown } from "$lib/actions/event";
  import Icon from "$atoms/Icon.svelte";

  const client = useQueryClient();

  const create = createMutation({
    mutationFn: (filter: MutableFilter) => postFilter(filter).fetch(),
    onSuccess: () => {
      client.invalidateQueries({
        queryKey: ["filters"],
      });
      value = "";
    },
  });

  let value = "";

  function handleClick() {
    $create.mutate({ regexp: value });
  }
</script>

<section class="w-full flex gap-2 items-center justify-center">
  <input
    type="text"
    class="input input-bordered w-full font-mono"
    placeholder="New Filter"
    bind:value
    use:enterdown={handleClick}
  />
  <button class="btn btn-square btn-outline btn-primary" on:click={handleClick}>
    <Icon name="add" />
  </button>
</section>
