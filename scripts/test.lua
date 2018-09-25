-- test.lua
config = {x=200, y=200, w=64, h=64, icon=":/res/images/cmd.png"}

function onLeftClick()
	os.execute("start notepad.exe")
	adiantum_switch_window()
end