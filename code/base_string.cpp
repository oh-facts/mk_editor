u64 str8_len(char *c)
{
  u64 out = 0;
  while(*c++)
  {
    out++;
  }
  return out;
}

Str8 str8(u8 *c, u64 len)
{
  Str8 out = 
  {
    c,len
  };
  return out;
}

void str8_cpy(Str8 *dst, Str8 *src)
{
  for(u32 i = 0; i < src->len; i ++)
  {
    dst->c[i] = src->c[i];
  }
}

Str8 push_str8(Arena *arena, const char *fmt, va_list args)
{
  Str8 out = {};
  
  va_list args_copy;
  va_copy(args_copy, args);
  
  int bytes_req = stbsp_vsnprintf(0, 0, fmt, args) + 1;
  
  out.c = push_array(arena, u8, bytes_req);
  out.len = stbsp_vsnprintf((char *)out.c, bytes_req, fmt, args_copy);
  va_end(args_copy);
  
  return out;
}