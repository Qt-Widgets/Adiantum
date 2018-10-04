-- cmd.lua
config = {x=200, y=200, w=64, h=64, icon="%APP_DIR%/res/images/cmd.png"}

function onLeftClick()
	os.execute("start cmd.exe")
	ext_switch_window()
end