LOGIN_LAYER_CLASS = class("LOGIN_LAYER_CLASS", function()
    return display.newScene("LOGIN_LAYER_CLASS")
end)


function LOGIN_LAYER_CLASS:ctor()
    self:enableNodeEvents()
    self.gameAllSize = 0
    self.gameCurSize = 0
    self:onInit()
end

function LOGIN_LAYER_CLASS:onInit()
    print("LOGIN_LAYER_CLASS init")
    display.newSprite("HelloWorld.png")
        :move(display.center)
        :addTo(self)

    cc.Label:createWithSystemFont("Hello World", "Arial", 40)
        :move(display.cx, display.cy + 200)
        :addTo(self)

    cc.Label:createWithSystemFont("用户:", "Arial", 30)
        :move(311, 199)
        :addTo(self)
        
    cc.Label:createWithSystemFont("密码:", "Arial", 30)
        :move(311, 155)
        :addTo(self)
       
    local res = "CyanSquare.png"
    local width = 200
    local height = 40
    local back = cc.Scale9Sprite:create(res)
    local account_eb = ccui.EditBox:create(cc.size(width,height),res)
    self:addChild(account_eb)
    account_eb:setPosition( cc.p(500,200) )
    account_eb:setPlaceHolder("click to input text")
    self.account_eb = account_eb

    local back = cc.Scale9Sprite:create(res)
    local passwd_eb = ccui.EditBox:create(cc.size(width,height),res)
    self:addChild(passwd_eb)
    passwd_eb:setPosition( cc.p(500,153) )
    passwd_eb:setPlaceHolder("click to input text")
    self.passwd_eb = passwd_eb


    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.began then
            print("Touch Down")
        elseif eventType == ccui.TouchEventType.moved then
            print("Touch Move")
        elseif eventType == ccui.TouchEventType.ended then
            print("Touch Up")
        elseif eventType == ccui.TouchEventType.canceled then
            print("Touch Cancelled")
        end
    end

    local textButton = ccui.Button:create()
    textButton:setTouchEnabled(true)
    textButton:setScale9Enabled(true)
    textButton:loadTextures("button.png", "buttonHighlighted.png", "")
    textButton:setContentSize(cc.size(180, textButton:getVirtualRendererSize().height * 1.5))
    textButton:setTitleText("登陆")
    textButton:setPosition(cc.p(500,100))
    textButton:addTouchEventListener(touchEvent)
    self:addChild(textButton)
end

function LOGIN_LAYER_CLASS:onCleanup_()

end