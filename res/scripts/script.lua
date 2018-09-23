-- script.lua

function onLeftClick()
	os.execute('start notepad.exe')
end

function onRequestDone(response)
	return [[done]]
end
