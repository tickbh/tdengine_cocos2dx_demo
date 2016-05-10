DDZ_DESK_LAYER_CLASS = class("DDZ_DESK_LAYER_CLASS", function()
    return display.newScene("DDZ_DESK_LAYER_CLASS")
end)


function DDZ_DESK_LAYER_CLASS:ctor()
    self:enableNodeEvents()
    self.uid = new_cookie()
    self.poker_list_spirte = {}
    self.count_downs = {}
    self.user_heads = {}
    --出牌的列表
    self.play_poker_lists = {}
    --底牌的列表
    self.own_poker_lists = {}
    self.ready_tip = {}
    self:onInit()
end

function DDZ_DESK_LAYER_CLASS:onInit()
    print("DDZ_DESK_LAYER_CLASS init")
    display.newSprite("kuang/ddz_bg.png")
        :move(display.center)
        :addTo(self)

    cc.Label:createWithSystemFont("Ddz Desk Layer", "Arial", 40)
        :move(display.cx, display.cy + 200)
        :addTo(self)

    self:add_ready_btn()
    self:add_choose_lord()
    self:add_cancel_lord()
    self:add_cancel_round()
    self:add_ok_round()
    self:add_tip_tbn()

    self:recover_first_status()

    self:add_count_downs()
    self:add_user_heads()
    self:add_ready_tip()

    self:add_listener()

    self:set_lord_type(false)
end

function DDZ_DESK_LAYER_CLASS:add_listener()
    -- handing touch events
    self.touchBeginPoint = nil
    local function onTouchBegan(touch, event)
        local location = touch:getLocation()
        self.touchBeginPoint = {x = location.x, y = location.y}
        return true
    end

    local function onTouchMoved(touch, event)
        local location = touch:getLocation()
    end

    local function onTouchEnded(touch, event)
        if not self.touchBeginPoint then
            return
        end
        local location = touch:getLocation()
        local touchRect = cc.rect(math.min(self.touchBeginPoint.x, location.x), math.min(self.touchBeginPoint.y, location.y),
            math.abs(self.touchBeginPoint.x - location.x), math.abs(self.touchBeginPoint.y - location.y))
        local is_move = touchRect.width > 30
        local pre_inter_rect = nil
        for i=#self.poker_list_spirte,1,-1 do
            local poker = self.poker_list_spirte[i]
            local rect = cc.rectIntersection(touchRect, poker:getRect())
            if rect.width >= 0 and rect.height >= 0 then
                if not pre_inter_rect or not is_rect_contains_rect(pre_inter_rect, rect) then
                    poker:reverse_select()
                end
                if not is_move then
                    break
                end
                pre_inter_rect = rect
            end
        end
        self.touchBeginPoint = nil
    end

    local listener = cc.EventListenerTouchOneByOne:create()
    listener:registerScriptHandler(onTouchBegan,cc.Handler.EVENT_TOUCH_BEGAN )
    listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
    listener:registerScriptHandler(onTouchEnded,cc.Handler.EVENT_TOUCH_ENDED )
    local eventDispatcher = self:getEventDispatcher()
    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, self)
end

function DDZ_DESK_LAYER_CLASS:add_ready_tip()
    local ready_pre = cc.Label:createWithSystemFont("准备", "Arial", 40)
        :move(cc.p(100, display.height * 0.75))
        :addTo(self)
    self.ready_tip["pre"] = ready_pre

    local ready_my = cc.Label:createWithSystemFont("准备", "Arial", 40)
        :move(cc.p(display.width / 2, 187))
        :addTo(self)
    self.ready_tip["my"] = ready_my

    local ready_after = cc.Label:createWithSystemFont("准备", "Arial", 40)
        :move(cc.p(display.width - 100, display.height * 0.75))
        :addTo(self)
    self.ready_tip["after"] = ready_after

    self:hide_all_ready_tip()
end

function DDZ_DESK_LAYER_CLASS:add_count_downs()
    local count_down_pre = COUNT_DOWN_CLASS:create({left_time = 0})
    count_down_pre:setPosition(cc.p(100, 544))
    count_down_pre:setAnchorPoint(cc.p(0.5, 0.5))
    self:addChild(count_down_pre)
    self.count_downs["pre"] = count_down_pre

    local count_down_my = COUNT_DOWN_CLASS:create({left_time = 0})
    count_down_my:setPosition(cc.p(313, 150))
    count_down_my:setAnchorPoint(cc.p(0.5, 0.5))
    self:addChild(count_down_my)
    self.count_downs["my"] = count_down_my 

    local count_down_after = COUNT_DOWN_CLASS:create({left_time = 0})
    count_down_after:setAnchorPoint(cc.p(0.5, 0.5))
    count_down_after:setPosition(cc.p(display.width - 100, 544))
    self:addChild(count_down_after)
    self.count_downs["after"] = count_down_after 

    self:hide_all_count_down()
end

function DDZ_DESK_LAYER_CLASS:add_user_heads()
    local function create_user_head(is_man, pos, anchor)
        local sprite = display.newSprite()
        if is_man then
            sprite:setTexture("icon/normal_head_man.png")
        else
            sprite:setTexture("icon/normal_head_woman.png")
        end
        sprite:setPosition(pos)
        if anchor then
            sprite:setAnchorPoint(anchor)
        end
        sprite:setScale(0.5)
        self:addChild(sprite)
        return sprite
    end
    self.user_heads["pre"] = create_user_head(true, cc.p(0, display.height), cc.p(0, 1))
    self.user_heads["my"] = create_user_head(true, cc.p(0, 200), cc.p(0, 0))
    self.user_heads["after"] = create_user_head(true, cc.p(display.width, display.height), cc.p(1, 1))
end



function DDZ_DESK_LAYER_CLASS:hide_all_ready_tip()
    for _,node in pairs(self.ready_tip) do
        node:hide()
    end
end

function DDZ_DESK_LAYER_CLASS:hide_all_count_down()
    for _,node in pairs(self.count_downs) do
        node:hide()
    end
end

function DDZ_DESK_LAYER_CLASS:add_ready_btn()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "ready"})
        end
    end

    local ready_btn = ccui.Button:create()
    ready_btn:setTouchEnabled(true)
    ready_btn:loadTextures("button/btn_confirm.png", "", "")
    ready_btn:setScale(0.6)
    ready_btn:setPosition(cc.p(display.width / 2,100))
    ready_btn:addTouchEventListener(touchEvent)
    self:addChild(ready_btn)
    self.ready_btn = ready_btn
end

function DDZ_DESK_LAYER_CLASS:add_choose_lord()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "choose", is_choose = 1})
        end
    end

    local choose_lord = ccui.Button:create()
    choose_lord:setTouchEnabled(true)
    choose_lord:loadTextures("button/btn_zi_jiaodizhu.png", "button/btn_zi_jiaodizhu1.png", "button/btn_zi_jiaodizhu2.png")
    choose_lord:setScale(0.6)
    choose_lord:setPosition(cc.p(440, 150))
    choose_lord:addTouchEventListener(touchEvent)
    self:addChild(choose_lord)
    self.choose_lord = choose_lord
end

function DDZ_DESK_LAYER_CLASS:add_cancel_lord()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "choose", is_choose = 0})
        end
    end

    local cancel_lord = ccui.Button:create()
    cancel_lord:setTouchEnabled(true)
    cancel_lord:loadTextures("button/btn_zi_bujiao.png", "button/btn_zi_bujiao1.png", "button/btn_zi_bujiao2.png")
    cancel_lord:setScale(0.6)
    cancel_lord:setPosition(cc.p(840, 150))
    cancel_lord:addTouchEventListener(touchEvent)
    self:addChild(cancel_lord)
    self.cancel_lord = cancel_lord
end

function DDZ_DESK_LAYER_CLASS:set_lord_type(is_first)
    if is_first then
        self.choose_lord:loadTextures("button/btn_zi_jiaodizhu.png", "button/btn_zi_jiaodizhu1.png", "button/btn_zi_jiaodizhu2.png")
        self.choose_lord:loadTextures("button/btn_zi_bujiao.png", "button/btn_zi_bujiao1.png", "button/btn_zi_bujiao2.png")
    else
        self.choose_lord:loadTextures("button/btn_zi_qiangdizhu.png", "button/btn_zi_qiangdizhu1.png", "button/btn_zi_qiangdizhu2.png")
        self.cancel_lord:loadTextures("button/btn_zi_buqiang.png", "button/btn_zi_buqiang1.png", "button/btn_zi_buqiang2.png")
    end
end

function DDZ_DESK_LAYER_CLASS:add_cancel_round()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "round", is_choose = 0})
        end
    end

    local cancel_round = ccui.Button:create()
    cancel_round:setTouchEnabled(true)
    cancel_round:setScale(0.6)
    cancel_round:loadTextures("button/btn_zi_buchu.png", "button/btn_zi_buchu1.png", "button/btn_zi_buchu2.png")
    cancel_round:setPosition(cc.p(440,150))
    cancel_round:addTouchEventListener(touchEvent)
    self:addChild(cancel_round)
    self.cancel_round = cancel_round
end


function DDZ_DESK_LAYER_CLASS:add_ok_round()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            local select_list = self:get_poker_select()
            if #select_list == 0 then
                trace("请选择您要出的牌")
                return
            end
            local poker_type = DDZ_D.get_card_type(select_list)
            if poker_type == DDZ_D.TYPE_ERROR then
                trace("您所出的牌不符合规则")
                return 
            end
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "deal_poker", poker_list = select_list})
        end
    end

    local ok_round = ccui.Button:create()
    ok_round:setTouchEnabled(true)
    ok_round:setScale(0.6)
    ok_round:loadTextures("button/btn_zi_chupai.png", "button/btn_zi_chupai1.png", "button/btn_zi_chupai2.png")
    ok_round:setPosition(cc.p(640,150))
    ok_round:addTouchEventListener(touchEvent)
    self:addChild(ok_round)
    self.ok_round = ok_round
end

function DDZ_DESK_LAYER_CLASS:add_tip_tbn()
    local function touchEvent(sender,eventType)
        if eventType == ccui.TouchEventType.ended then
            ME_D.request_message(CMD_ROOM_MESSAGE, "desk_op", {oper = "round", is_choose = 0})
        end
    end

    local tip_tbn = ccui.Button:create()
    tip_tbn:setTouchEnabled(true)
    tip_tbn:setScale(0.6)
    tip_tbn:loadTextures("button/btn_zi_tip.png", "button/btn_zi_tip1.png", "button/btn_zi_tip2.png")
    tip_tbn:setContentSize(cc.size(80, tip_tbn:getVirtualRendererSize().height * 1.5))
    tip_tbn:setPosition(cc.p(840,150))
    tip_tbn:addTouchEventListener(touchEvent)
    self:addChild(tip_tbn)
    self.tip_tbn = tip_tbn
end

function DDZ_DESK_LAYER_CLASS:get_poker_select()
    local select_ids = {}
    for _,poker in ipairs(self.poker_list_spirte) do
        if poker:get_select() then
            table.insert(select_ids, poker:get_data_id())
        end
    end
    return select_ids
end

function DDZ_DESK_LAYER_CLASS:enter_desk()
    trace("DDZ_DESK_LAYER_CLASS:enter_desk")
end

function DDZ_DESK_LAYER_CLASS:remove_all_poker()
    for _,poker in ipairs(self.poker_list_spirte) do
        poker:removeFromParent()
    end
    self.poker_list_spirte = {}
end

function DDZ_DESK_LAYER_CLASS:get_my_idx()
    self.desk_info = self.desk_info or {}
    trace("desk_info is %o", self.desk_info)
    for idx,info in pairs(self.desk_info.wheels or {}) do
        if info.rid == ME_D.get_rid() then
            return idx
        end
    end
    return -1
end

function DDZ_DESK_LAYER_CLASS:get_idx_info(idx)
    return self.desk_info.wheels[idx]
end

function DDZ_DESK_LAYER_CLASS:get_my_poker_list()
    self.desk_info = self.desk_info or {}
    trace("desk_info is %o", self.desk_info)
    for idx,info in pairs(self.desk_info.wheels or {}) do
        if info.rid == ME_D.get_rid() then
            return info.poker_list
        end
    end
    return {}
end

function DDZ_DESK_LAYER_CLASS:hide_all_btn()
    self.ready_btn:setVisible(false)
    self.choose_lord:setVisible(false)
    self.cancel_lord:setVisible(false)
    self.cancel_round:setVisible(false)
    self.ok_round:setVisible(false)
    self.tip_tbn:setVisible(false)
end

function DDZ_DESK_LAYER_CLASS:recover_first_status() 
    self:remove_all_poker()
    self:hide_all_btn()
    self:hide_all_count_down()
    self:hide_all_ready_tip()
    self.ready_btn:setVisible(true)
end

function DDZ_DESK_LAYER_CLASS:calc_idx_tag(idx)
    local my_idx = self:get_my_idx()
    if my_idx == idx then
        return "my"
    elseif (my_idx + 1) % 3 == idx % 3 then
        return "after"
    elseif (my_idx - 1 + 3) % 3 == idx % 3 then
        return "pre"
    else
        return "unknow"
    end
end

function DDZ_DESK_LAYER_CLASS:show_count_down_tag(idx, left_time)
    self:hide_all_count_down()
    local tag = self:calc_idx_tag(idx)
    trace("DDZ_DESK_LAYER_CLASS:show_count_down_tag tag = %o idx = %o, count_downs = %o", tag, idx, self.count_downs)
    local count_down = self.count_downs[tag]
    if count_down then
        count_down:set_left_time(left_time)
        count_down:show()
    end
end

function DDZ_DESK_LAYER_CLASS:show_ready_status(idx)
    local tag = self:calc_idx_tag(idx)
    trace("DDZ_DESK_LAYER_CLASS:show_ready_status tag = %o", tag)
    local ready_tip = self.ready_tip[tag]
    if ready_tip then
        ready_tip:show()
    end
end

function DDZ_DESK_LAYER_CLASS:hide_play_poker(idx)
    local tag = self:calc_idx_tag(idx)
    local list = self.play_poker_lists[tag]
    if list then
        for _,v in ipairs(list) do
            v:removeSelf()
        end
        self.play_poker_lists[tag] = {}
    end
end

function DDZ_DESK_LAYER_CLASS:clear_play_poker()
    for i=1,3 do
        self:hide_play_poker(i)
    end
end

function DDZ_DESK_LAYER_CLASS:show_play_poker(idx, poker_list)
    self:hide_play_poker(idx)
    local tag = self:calc_idx_tag(idx)
    if not poker_list or #poker_list == 0 then
        if tag == "pre" or tag == "after" then
            local start_y = 600
            local start_x = 200
            local anchor_point = cc.p(0, 1)
            if tag == "after" then
                start_x = display.width - start_x
                anchor_point.x = 1
            end
            local label = cc.Label:createWithSystemFont("不出", "Arial", 40)
                :move(start_x, start_y)
                :addTo(self)
            self.play_poker_lists[tag] = { label }
        else
            local half_width = display.width / 2
            local len = sizeof(poker_list)
            local step_x = 20
            local start_x = half_width
            local label = cc.Label:createWithSystemFont("不出", "Arial", 40)
                :move(start_x, 200)
                :addTo(self)
            self.play_poker_lists["my"] = { label }
        end
        return
    end

    if tag == "pre" or tag == "after" then
        local start_y = 600
        local start_x = 200
        local anchor_point = cc.p(0, 1)
        if tag == "after" then
            start_x = display.width - start_x
            anchor_point.x = 1
        end
        local sprite_list = {}
        for i,id in ipairs(poker_list) do
            local poker = POKER_SPRITE_CLASS:create({id=id})
            poker:setPosition(cc.p(start_x,start_y - 20 * (i - 1)))
            poker:setAnchorPoint(anchor_point)
            self:addChild(poker)
            table.insert(sprite_list, poker)
        end
        self.play_poker_lists[tag] = sprite_list
    else
        local half_width = display.width / 2
        local len = sizeof(poker_list)
        local step_x = 20
        local start_x = half_width - len / 2 * step_x
        local sprite_list = {}
        for i,poker in ipairs(poker_list or {}) do
            local poker = POKER_SPRITE_CLASS:create({id=poker})
            poker:setPosition(cc.p(start_x + step_x * (i - 1), 200))
            poker:setAnchorPoint(cc.p(0.5, 0))
            self:addChild(poker)
            table.insert(sprite_list, poker)
        end
        self.play_poker_lists["my"] = sprite_list
    end
end


function DDZ_DESK_LAYER_CLASS:turn_index(idx)
    if self.cur_step == DDZ_STEP_LORD then
        self:hide_all_btn()
        if idx == self:get_my_idx() then
            self.choose_lord:setVisible(true)
            self.cancel_lord:setVisible(true)
        end
        self:show_count_down_tag(idx, 10)
    elseif self.cur_step == DDZ_STEP_PLAY then
        self:hide_all_btn()
        if idx == self:get_my_idx() then
            self.cancel_round:setVisible(true)
            self.ok_round:setVisible(true)
            self.tip_tbn:setVisible(true)
        end
        self:show_count_down_tag(idx, 30)
        self:hide_play_poker(idx)
    end
end

function DDZ_DESK_LAYER_CLASS:turn_step(step)
    self.cur_step = step
    if self.cur_step == DDZ_STEP_NONE then
        self:recover_first_status()
    elseif self.cur_step == DDZ_STEP_LORD then
        self:hide_all_ready_tip()
    end
end

function DDZ_DESK_LAYER_CLASS:reset_by_poker_list(poker_list)
    self:remove_all_poker()
    local half_width = display.width / 2
    local len = sizeof(poker_list)
    local step_x = 30
    local start_x = half_width - len / 2 * step_x
    for i,poker in ipairs(poker_list or {}) do
        local poker = POKER_SPRITE_CLASS:create({id=poker})
        poker:setPosition(cc.p(start_x + step_x * i,0))
        poker:setAnchorPoint(cc.p(0.5, 0))
        self:addChild(poker)
        table.insert(self.poker_list_spirte, poker)
    end
end

function DDZ_DESK_LAYER_CLASS:hide_own_poker(idx)
    local tag = self:calc_idx_tag(idx)
    local list = self.own_poker_lists[tag]
    if list then
        for _,v in ipairs(list) do
            v:removeSelf()
        end
        self.own_poker_lists[tag] = {}
    end
end

function DDZ_DESK_LAYER_CLASS:clear_own_poker()
    for i=1,3 do
        self:hide_own_poker(i)
    end
end

function DDZ_DESK_LAYER_CLASS:show_own_poker(idx, poker_list, poker_num)
    self:hide_own_poker(idx)
    local tag = self:calc_idx_tag(idx)
    trace("self.own_poker_lists = %o tag = %o", self.own_poker_lists, tag)
    if not poker_list or #poker_list == 0 then
        if tag == "pre" or tag == "after" then
            local start_y = display.height - 100
            local start_x = 0
            local anchor_point = cc.p(0, 1)
            if tag == "after" then
                start_x = display.width - start_x
                anchor_point.x = 1
            end

            local poker = POKER_SPRITE_CLASS:create({is_back=true, left_num = poker_num or 10})
            poker:setAnchorPoint(anchor_point)
            poker:setPosition(cc.p(start_x,start_y))
            self:addChild(poker)

            self.own_poker_lists[tag] = { poker }
        end

        return
    end

    if tag == "pre" or tag == "after" then
        local start_y = display.height - 100
        local start_x = 0
        local anchor_point = cc.p(0, 1)
        if tag == "after" then
            start_x = display.width - start_x
            anchor_point.x = 1
        end
        local sprite_list = {}
        for i,id in ipairs(poker_list) do
            local poker = POKER_SPRITE_CLASS:create({id=id})
            poker:setPosition(cc.p(start_x,start_y - 20 * (i - 1)))
            poker:setAnchorPoint(anchor_point)
            self:addChild(poker)
            table.insert(sprite_list, poker)
        end
        self.own_poker_lists[tag] = sprite_list
    else
        local half_width = display.width / 2
        local len = sizeof(poker_list)
        local step_x = 30
        local start_x = half_width - len / 2 * step_x
        local sprite_list = {}
        for i,poker in ipairs(poker_list or {}) do
            local poker = POKER_SPRITE_CLASS:create({id=poker})
            poker:setPosition(cc.p(start_x + step_x * (i - 1),0))
            poker:setAnchorPoint(cc.p(0.5, 0))
            self:addChild(poker)
            table.insert(sprite_list, poker)
        end
        self.own_poker_lists["my"] = sprite_list
    end
end

function DDZ_DESK_LAYER_CLASS:room_msg_receive(user, oper, info)
    trace("DDZ_DESK_LAYER_CLASS:room_msg_receive %o %o", oper, info)
    if oper == "poker_init" then
        local idx_info = self:get_idx_info(info.idx)
        self:reset_by_poker_list(info.poker_list)
        idx_info.poker_list = info.poker_list
    elseif oper == "success_user_ready" then
        if info.rid == ME_D.get_rid() then
            self.ready_btn:setVisible(false)
        end
        self:show_ready_status(info.idx)
    elseif oper == "step_change" then
        self:turn_step(info.cur_step)
    elseif oper == "desk_info" then
        self.desk_info = info
        self:turn_step(info.cur_step or DDZ_STEP_NONE)
        self:turn_index(info.cur_op_idx or -1)
        if info.cur_step ~= DDZ_STEP_NONE then
            self:reset_by_poker_list(self:get_my_poker_list())
        end
    elseif oper == "op_idx" then
        self:turn_index(info.cur_op_idx)
    elseif oper == "deal_poker" then
        if info.idx == self:get_my_idx() then
            if info.is_play == 1 or (info.poker_list and #info.poker_list > 0) then
                local idx_info = self:get_idx_info(info.idx)
                local success, new_poker_list = DDZ_D.sub_poker(idx_info.poker_list, info.poker_list)
                if success then
                    idx_info.poker_list = new_poker_list
                    self:reset_by_poker_list(idx_info.poker_list)
                end
            end
        end
        self:show_play_poker(info.idx, info.poker_list)
    end
end

function DDZ_DESK_LAYER_CLASS:onCleanup_()

end

function DDZ_DESK_LAYER_CLASS:onEnter_()
    register_as_audience(self.uid, {EVENT_ENTER_DESK={func = self.enter_desk, args = {self} }})
    register_as_audience(self.uid, {EVENT_ROOM_MSG_RECEIVE={func = self.room_msg_receive, args = {self} }})

    self:show_play_poker(1, {1, 2, 3, 4})
    self:show_play_poker(2, {5, 6, 7, 8})
    self:show_play_poker(3, {})

    -- self:show_own_poker(4, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
    self:show_own_poker(1, {}, 10)
    self:show_own_poker(3, {}, 10)
    self:show_own_poker(2, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
    self:show_own_poker(1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
    self:show_own_poker(3, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
end

function DDZ_DESK_LAYER_CLASS:onExit_()
    remove_audience_from_raiser(self.uid)
end