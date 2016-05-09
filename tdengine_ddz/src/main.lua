
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
    package.loaded[full_name] = false
    require(full_name)
    -- 回收垃圾
    collectgarbage("collect")
end

g_scene = nil
update("global/base/util")

local function main()
    update("global/base/load_folder")
    load_folder("global/include")
    load_folder("global/base")
    load_folder("global/inherit")
    load_folder("global/clone")
    load_folder("global/daemons")

    load_folder("share")

    update("etc/format")
    load_folder("client/include")
    load_folder("client/global","md5")
    load_folder("client/clone")
    load_folder("client/msgs")
    update("client/daemons/logind")
    update("client/daemons/med")
    update("client/daemons/ddz_d")

    load_folder("ui")

    post_init()
    local  login_scene = MAIN_SCENE_CLASS:create()
    display.runScene(login_scene)
    g_scene = login_scene
end

local status, msg = xpcall(main, error_handle)
if not status then
    print(msg)
end
