
if not bit32 then
    bit32 = bit
end

trace("md5_core = %o", md5_core)
if not calc_str_md5 then
    calc_str_md5 = md5_core.sumhexa
end