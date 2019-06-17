-- internet.lua
config = {
	modules = {"string", "json"},
	x=456,
	y=200,
	w=256,
	h=64,
	url="http://ip-api.com/json/",
	update_interval=60000
}

function onLeftClick()
end

function onUpdate()
	local response = ext_network_request(config["url"])
	local image = "on"
	local address = "No internet connection"
	local geo = "-/-"
	if response == "NETWORK_ERROR_FLAG" then
		image = "off"
	else
		data = json.decode(response)
		address = data["query"]
		geo = data["country"]..", "..data["regionName"]..", "..data["city"]
	end
	local result = [[<style>
		 .first {padding-left:4px;color: white;vertical-align:middle;}
		 .icon {padding-left:4px;}.addr {font-weight:bold;font-size: 14px;}.geo {font-size: 10px;}
		 </style>
		 <table width='100%'><tr><td align='left' width='48px' class='icon'><img src='%APP_DIR%/res/images/internet/]]..image..[[.png'></td><td colspan="4" class='first'><span class='addr'>]]..address..[[</span><br><span class='geo'>]]..geo..[[</span></td></tr></table>]]
	return result
end