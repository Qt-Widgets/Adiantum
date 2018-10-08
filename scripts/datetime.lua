-- datetime.lua
config = {
	modules = {"os"},
	x=264,
	y=264,
	w=192,
	h=64,
	loader_enabled=false,
	manual_refresh_enabled=false,
	update_interval=1000
}

function onLeftClick()
end

function onUpdate()
	local daysoftheweek = {"Sunday","Monday","Tuesday","Wednesday","Thrusday","Friday","Saturday"}
	local monthsoftheyear = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"}
	local date_table = os.date("*t")
	local day = daysoftheweek[date_table.wday]
	local month = monthsoftheyear[date_table.month]
	result = [[<style>.first {padding-left:8px;color: white;vertical-align:middle;}.second {color: white;padding-right:8px;vertical-align:middle;}
             .month {font-weight:bold;font-size: 14px;}.day {font-size: 10px;}.time {font-size: 24px;}</style>
             <table width='100%'><tr>
             <td class='first'><span class='month'>]]..month..[[, ]]..date_table.day..[[</span><br><span class='day'>]]..day..[[</span></td>
             <td align='right' class='second'><span class='time'>]]..date_table.hour..[[:]]..date_table.min..[[</span></td></tr></table>]]
	return result
end
