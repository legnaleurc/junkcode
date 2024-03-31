<script lang="ts">
  import { createQuery } from "@tanstack/svelte-query";

  import { getFilterList } from "$lib/api/filters";
  import FilterItem from "$molecules/FilterItem.svelte";

  const read = createQuery({
    queryKey: ["filters"],
    queryFn: () => getFilterList().fetch(),
  });
</script>

<section class="w-full flex flex-col gap-2">
  {#if $read.isPending}
    <span class="loading loading-spinner loading-lg"></span>
  {:else if $read.error}
    <div role="alert" class="alert alert-error">
      <span>I AM ERROR</span>
    </div>
  {:else if $read.isSuccess}
    {@const filterList = $read.data}
    {#each filterList as filter (filter.id)}
      <FilterItem id={filter.id} regexp={filter.regexp} />
    {/each}
  {/if}
</section>
