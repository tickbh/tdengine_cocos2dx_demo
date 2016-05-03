
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

update("global/base/util")

local function main()
    update("global/base/load_folder")
    load_folder("ui")

    local  login_scene = MAIN_SCENE_CLASS:create()
    display.runScene(login_scene)
end

local status, msg = xpcall(main, error_handle)
if not status then
    print(msg)
end
