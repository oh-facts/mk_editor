void* _arena_alloc(Arena* arena, size_t size)
{
  Assert(arena->used + size <= arena->size);
	
  void* out = arena->base + arena->used;
  
  size_t mod = (size_t)out & (arena->align - 1);
	
  if(mod != 0)
  {
		size_t adjustment = arena->align - mod;
    out = (u8*)out + adjustment;
		arena->used += adjustment;
  }
	
  arena->used += size;
  return out;
}

Arena_temp arena_temp_begin(Arena *arena)
{
  Arena_temp out = {
    .arena = arena,
    .pos = arena->used,
  };
  return out;
}

void arena_temp_end(Arena_temp *temp)
{
  temp->arena->used = temp->pos;
}

void arena_innit(Arena* arena, size_t size, void* base)
{
  arena_innit_align(arena, size, base, DEFAULT_ALIGN);
}

void arena_innit_align(Arena *arena, size_t size, void *base, u64 align)
{
	arena->size = size;
  arena->base = (u8*)base;
	arena->align = align;
  arena->used = 0;
}

void mem_cpy(void *dst, void *src, size_t size)
{
  u8 *_dst = (u8*)dst;
  u8 *_src = (u8*)src;
  
  for(u32 i = 0; i < size; i ++)
  {
    _dst[i] = _src[i];
  }
}
