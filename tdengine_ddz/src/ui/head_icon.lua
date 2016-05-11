HEAD_ICON_CLASS = class("HEAD_ICON_CLASS", function()
    return display.newNode()
end)

function HEAD_ICON_CLASS:ctor(data)
    self.data = {}
    local head_bg = display.newSprite()
    head_bg:setTexture("icon/icon_bg.png")
    head_bg:setAnchorPoint(cc.p(0, 0))
    self:addChild(head_bg)
    self.head_bg = head_bg
    self:setContentSize(head_bg:getContentSize())

    local sprite = display.newSprite()
    sprite:setAnchorPoint(cc.p(0, 0))
    self:addChild(sprite)
    self.sprite = sprite
    self:set_sprite_by_data(data)
end

function HEAD_ICON_CLASS:set_sprite_by_data(data)
    merge(self.data, data)
    --是否为地主确定阶段
    local sprite_path = ""
    if self.data.is_in_lord then
        if self.data.is_lord then
            if self.data.is_man then
                sprite_path = "icon/dz_head_man.png"
            else
                sprite_path = "icon/dz_head_woman.png"
            end
        else
            if self.data.is_man then
                sprite_path = "icon/nm_head_man.png"
            else
                sprite_path = "icon/nm_head_woman.png"
            end
        end
    else
        if self.data.is_man then
            sprite_path = "icon/normal_head_man.png"
        else
            sprite_path = "icon/normal_head_woman.png"
        end
    end

    if self.data.is_leave then
        self.sprite:hide()
    else
        self.sprite:show()
        self.sprite:setTexture(sprite_path)
    end
    
end
