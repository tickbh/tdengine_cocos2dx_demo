POKER_SPRITE_CLASS = class("POKER_SPRITE_CLASS", function()
    return display.newNode()
end)

function POKER_SPRITE_CLASS:ctor(data)
    self.sprite = display.newSprite():addTo(self)
    self.sprite:setAnchorPoint(cc.p(0, 0))
    self.is_select = false
    if data.is_back then
        self.sprite:setTexture("res/poker/poker__back.jpg")
    else
        self.sprite:setTexture(string.format("res/poker/poker__%02X.jpg", data.id))
    end

end

--被选中则上移20px
function POKER_SPRITE_CLASS:set_select(is_select)
    if is_select then
        self.sprite:setPositionY(20)
    else
        self.sprite:setPositionY(0)
    end
    self.is_select = is_select
end

function POKER_SPRITE_CLASS:reverse_select()
    local is_select = not self.is_select
    self:set_select(is_select)
end

function POKER_SPRITE_CLASS:getContentSize()
    return self.sprite:getContentSize()
end

function POKER_SPRITE_CLASS:getRect()
    local x, y = self:getPosition()
    local size = self:getContentSize()
    return cc.rect(x, y, size.width, size.height)
end
