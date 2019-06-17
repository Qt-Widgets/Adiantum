# Adiantum
QT desktop widget layer

Widget logic uses embedded Lua: https://www.lua.org/

Used Lua C++ wrapper: Sol2 by ThePhD: https://github.com/ThePhD/sol2

Resources:
* Icons and images by https://icons8.com/
* Weather widget example powered by MetaWeather public API https://www.metaweather.com/
* Internet widget example powered by IP Geolocation API IP-API http://ip-api.com/

Dependency management:
* Unpack Lua binaries from http://luabinaries.sourceforge.net/download.html to lib/lua directory
* Copy single-header library sol.hpp from https://github.com/ThePhD/sol2/tree/develop/single/sol to lib/sol directory

Lua libraries:
* json.lua - A lightweight JSON library for Lua by rxi: https://github.com/rxi/json.lua
