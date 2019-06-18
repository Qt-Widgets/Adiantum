-- usb.lua
config = {
	modules = {"string", "util", "math", "table", "json"},
	w=192,
	h=64,
	loader_enabled=false,
	manual_refresh_enabled=false,
	update_interval=10000
}

function onLeftClick()
end

function bytesToReadable(n)
	if util.isEmpty(n) then
		return 'N/A'
	end
	local d = {"", "K", "M", "G", "T", "P", "E", "Z", "Y"}
	local i = 1
	while(tonumber(n) > 1024)
	do
		i = i + 1
		n = n/1024
	end
	return util.floorToDigits(n, 2) .. d[i] .. "B"
end

function onUpdate()
	local response = ext_execute_process("WMIC logicaldisk where \"DriveType=2\" list brief /format:csv")
	local devices_count = 0;
	local result = [[<style>
		 .first {padding-left:4px;color: white;vertical-align:middle;}
		 .icon {padding-left:4px;}.name {font-weight:bold;font-size: 14px;}.info {font-size: 10px;}
		 </style>
		 <table width='100%'>]]
	if string.match(response, "DeviceID") then
		local lines = util.split(response, "\r\n")
		for key,value in pairs(lines) do	
			local items = util.split(lines[key], ",")
			if util.tableLength(items) == 7 then
				if items[1] ~= "Node" then
					devices_count = devices_count + 1
					local name = "Unnamed device"
					items[7] = items[7]:gsub("%\r", "")
					if not util.isEmpty(items[7]) then
						name = items[7]
					end
					local perc = "N/A"
					if not util.isEmpty(items[4]) and not util.isEmpty(items[6]) then
						perc = 100 * util.floorToDigits(items[4]/items[6], 2)
					end
					local info = items[2] .. " / " .. perc .. "% free of " .. bytesToReadable(items[6])
					result = result .. [[<tr><td align='left' width='48px' class='icon'><img src='%APP_DIR%/res/images/usb/on.png'></td><td colspan="4" class='first'><span class='name'>]]..name..[[</span><br><span class='info'>]]..info..[[</span></td></tr>]]
				end
			end
		end
	else
		result = result .. [[<tr><td align='left' width='48px' class='icon'><img src='%APP_DIR%/res/images/usb/off.png'></td><td colspan="4" class='first'><span class='name'>No USB devices</span><br><span class='addr'>-/-</span></td></tr>]]
	end
	if devices_count > 1 then
		config["h"] = 64 + (32 * (devices_count - 1));
	else
		config["h"] = 64;
	end
	result = result .. [[</table>]]
	return result
end
