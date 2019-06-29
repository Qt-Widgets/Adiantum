-- chart.lua
config = {
	modules = {"string", "math", "util", "table", "json"},
	w=256,
	h=64,
	chart_enabled = true,
	chart_series = {},
	chart_counter = 1,
	chart_size = 20,
	manual_refresh_enabled=false,
	loader_enabled=false,
	update_interval=2000
}

function onLeftClick()
end

function onUpdate()
	local response = ext_execute_process("WMIC cpu get loadpercentage")
	if string.match(response, "LoadPercentage") then
		local lines = util.split(response, "\r\n")
		table.insert(config["chart_series"], {config["chart_counter"], tonumber(lines[2])})
		config["chart_counter"] = config["chart_counter"] + 1
		if (#config["chart_series"] > config["chart_size"]) then
			table.remove(config["chart_series"], 1)
		end
	end
	local result = [[<style>
		 .name {color: white; padding-right:8px; vertical-align:top; padding-top:0px; font-size: 10px;}
		 </style>
		 <table width='100%'><tr>
		 <td align='left' class='legend'></td>
         <td align='right' class='name'>CPU LOAD</td></tr></table>]]
	return result
end
