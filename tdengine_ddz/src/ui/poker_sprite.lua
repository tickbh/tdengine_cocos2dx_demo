POKER_SPRITE_CLASS = class("POKER_SPRITE_CLASS", function()
    return display.newSprite()
end)

function POKER_SPRITE_CLASS:ctor(data)
    if data.is_back then
        self:setTexture("res/poker/poker__back.jpg")
    else
        self:setTexture(string.format("res/poker/poker__%02X.jpg", data.id))
    end
    
end
