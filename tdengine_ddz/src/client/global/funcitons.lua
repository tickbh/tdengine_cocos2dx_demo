HEARTBEAT_INTERVAL = 10000

local seq_ids = {}

local seq_id = 0;

function get_network_seq_id(port_no)
    seq_ids[port_no] = (seq_ids[port_no] or 0) + 111
    seq_ids[port_no] = bit32.band(seq_ids[port_no], 0xFFFF) 
    return seq_ids[port_no];
end

function set_network_seq_id(port_no, seq)
    seq_ids[port_no] = seq
end

function get_node_rect(node)
    local x, y = node:getPosition()
    local size = node:getContentSize()
    local anchor = node:getAnchorPoint()
    local scaleX = node:getScaleX()
    local scaleY = node:getScaleY()
    local width = size.width * scaleX
    local height = size.height * scaleY
    local startX = x - anchor.x * width
    local startY = y - anchor.y * height
    return cc.rect(startX, startY, width, height)
end