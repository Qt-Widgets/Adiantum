--
-- util.lua
--
-- Miscellaneous helper functions

local util = {}

function util.tableLength(T)
	local count = 0
	for _ in pairs(T) do count = count + 1 end
	return count
end

function util.isEmpty(s)
	return s == nil or s == ''
end

function util.floorToDigits(n, k)
	local mult = math.pow(10, k)
	return math.floor(n * mult) / mult
end

function util.split(input, inSplitPattern, outResults)
	if not outResults then
		outResults = {}
	end
	local theStart = 1
	local theSplitStart, theSplitEnd = string.find(input, inSplitPattern, theStart)
	while theSplitStart do
		table.insert(outResults, string.sub(input, theStart, theSplitStart-1))
		theStart = theSplitEnd + 1
		theSplitStart, theSplitEnd = string.find(input, inSplitPattern, theStart)
	end
	table.insert(outResults, string.sub(input, theStart))
	return outResults
end

return util