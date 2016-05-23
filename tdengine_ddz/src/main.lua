
cc.FileUtils:getInstance():setPopupNotify(false)
cc.FileUtils:getInstance():addSearchPath("../../src/")
cc.FileUtils:getInstance():addSearchPath("../../res/")
cc.FileUtils:getInstance():addSearchPath("src/")
cc.FileUtils:getInstance():addSearchPath("res/")

require "config"
require "cocos.init"

-- 更新一个文件，强制重新载入
function update(name)
    name = string.gsub(name, ".lua", "") .. ".lua"
    local full_name = get_full_path(name)
    print("load file " .. full_name)
    package.loaded[full_name] = false
    require(full_name)
    -- 回收垃圾
    collectgarbage("collect")
end

g_scene = nil

update("global/base/util")
local function load_base()
    update("global/base/class")
    update("global/base/global_boot")
    update("global/base/global_frame")
    update("global/base/raiser")
    update("global/base/game_util")
    update("global/base/global_comm")
    update("global/base/heartbeat")
    update("global/base/rid")
    update("global/base/global_agents")
    update("global/base/global_cookie")
    update("global/base/load_folder")
    update("global/base/string_util")
    update("global/base/global_basic")
    update("global/base/global_data_boot")
    update("global/base/log")
end

local function load_files()
    load_base()
    update("global/include/define")

    update("global/inherit/agent")
    update("global/inherit/attrib")
    update("global/inherit/dbase")
    update("global/inherit/heartbeat")
    update("global/inherit/property")
    update("global/inherit/rid")


    update("global/clone/equip")
    update("global/clone/item")
    update("global/clone/queue")

    update("global/daemons/base64d")
    update("global/daemons/forbidden_wordd")
    update("global/daemons/importd")
    update("global/daemons/memoryd")
    update("global/daemons/propertyd")

    update("share/net")
    update("etc/format")


    update("client/include/define")

    update("client/global/md5")
    update("client/global/client_util")
    update("client/global/funcitons")


    update("client/clone/player")
    update("client/msgs/msg_client")

    update("client/daemons/logind")
    update("client/daemons/med")
    update("client/daemons/ddz_d")

    update("ui/count_down")
    update("ui/ddz_desk_layer")
    update("ui/ddz_final_layer")
    update("ui/ddz_room_layer")
    update("ui/head_icon")
    update("ui/login_layer")
    update("ui/main_layer")
    update("ui/main_scene")
    update("ui/poker_sprite")
    update("ui/user_detail_info")
end

local function main()
    load_files()

    send_debug_on(1)
    debug_on(1)

    post_init()
    local  login_scene = MAIN_SCENE_CLASS:create()
    display.runScene(login_scene)
    g_scene = login_scene
end

local status, msg = xpcall(main, error_handle)
if not status then
    print(msg)
end
