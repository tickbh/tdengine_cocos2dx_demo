POKER_SPRITE_CLASS = class("POKER_SPRITE_CLASS", function()
    return display.newNode()
end)

function POKER_SPRITE_CLASS:ctor(data)
    self.sprite = display.newSprite():addTo(self)
    self.sprite:setAnchorPoint(cc.p(0, 0))
    self.is_select = false
    self.data = data
    self:set_sprite_by_data(data)
    self:setContentSize(self.sprite:getContentSize())
    local size = self.sprite:getContentSize()
    self.label = cc.Label:createWithSystemFont("", "Arial", 20):move(size.width / 2, size.height / 2):addTo(self)
    self.label:setAnchorPoint(cc.p(0.5, 0.5))

    if (self.data.left_num or 0) ~= 0 then
        self.label:setString(tostring(self.data.left_num))
    end
end

function POKER_SPRITE_CLASS:set_sprite_by_data(info)
    if info.is_back then
        self.sprite:setTexture("res/poker/poker__back.jpg")
    else
        self.sprite:setTexture(string.format("res/poker/poker__%02X.jpg", info.id))
    end
    merge(self.data, info)
end

--被选中则上移20px
function POKER_SPRITE_CLASS:set_select(is_select)
    if is_select then
        self:setPositionY(20)
    else
        self:setPositionY(0)
    end
    self.is_select = is_select
end

function POKER_SPRITE_CLASS:get_select()
    return self.is_select
end

function POKER_SPRITE_CLASS:get_data_id()
    return self.data.id
end

function POKER_SPRITE_CLASS:reverse_select()
    local is_select = not self.is_select
    self:set_select(is_select)
end

function POKER_SPRITE_CLASS:getContentSize()
    return self.sprite:getContentSize()
end

function POKER_SPRITE_CLASS:setLeftPoker(left_num)
    self.data.left_num = left_num
    self.label:setString(tostring(left_num))
end
