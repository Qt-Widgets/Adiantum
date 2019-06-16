-- interfaces.lua
config = {
	modules = {"string", "json"},
	x=264,
	y=328,
	w=256,
	h=64,
	update_interval=5000
}

function onLeftClick()
end

function onUpdate()
	local response = ext_get_network_interfaces()
	if response == "" then
		return "";
	end
	local interfaces = json.decode(response)
	interfaces_count = 0;
	local result = [[<style>
		 .first {padding-left:4px;color: white;vertical-align:middle;}
		 .icon {padding-left:4px;}.name {font-weight:bold;font-size: 14px;}.addr {font-size: 10px;}
		 </style>
		 <table width='100%'>]]
	for key,value in pairs(interfaces) do
		interfaces_count = interfaces_count + 1
		result = result .. [[<tr><td align='left' width='48px' class='icon'><img src='%APP_DIR%/res/images/interfaces/]]..value["type"]..[[.png'></td><td colspan="4" class='first'><span class='name'>]]..value["name"]..[[</span><br><span class='addr'>]]..value["address"]..[[</span></td></tr>]]
	end
	config["h"] = 64 + (32 * (interfaces_count - 1));
	result = result .. [[</table>]]
	return result
end
