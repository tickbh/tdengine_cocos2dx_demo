
if not bit32 then
    bit32 = bit
end

trace("md5_core = %o", md5_core)
if not calc_str_md5 then
    calc_str_md5 = md5_core.sumhexa
end

function is_rect_contains_rect(rect1, rect2)
    local ret = false

    if (rect1.x <= rect2.x) and (rect1.x + rect1.width >= rect2.x + rect2.width) and
       (rect1.y <= rect2.y) and (rect1.y + rect1.height >= rect2.y + rect2.height) then
        ret = true
    end

    return ret
end