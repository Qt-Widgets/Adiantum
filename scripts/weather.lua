-- weather.lua
config = {
	modules = {"string", "math", "utf8", "json"},
	x=264,
	y=200,
	w=192,
	h=64,
	url="https://www.metaweather.com/api/location/2122265/",
	update_interval=60000
}

function onLeftClick()
end

function onUpdate()
	local response = ext_network_request(config["url"])
	local data = json.decode(response)
	weather = data["consolidated_weather"][1]["weather_state_name"]
	icon = data["consolidated_weather"][1]["weather_state_abbr"]
	title = data["title"]
	temp = math.floor(data["consolidated_weather"][1]["the_temp"])
	result = [[<style>.icon {padding-left:4px;}.first {color: white;vertical-align:middle;}.second {color: white;padding-right:4px;vertical-align:middle;}
             .city {font-weight:bold;font-size: 14px;}.state {font-size: 10px;}.temp {font-size: 24px;}</style>
             <table width='100%'><tr><td align='left' width='48px' class='icon'><img src='%APP_DIR%/res/images/weather/]]..icon..[[.png'></td>
             <td class='first'><span class='city'>]]..title..[[</span><br><span class='state'>]]..weather..[[</span></td>
             <td align='right' class='second'><span class='temp'>]]..temp..utf8.char(0x00B0)..[[</span></td></tr></table>]]
	return result
end
