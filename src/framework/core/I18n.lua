local M = Class()

local function loadlang(path)
	local f = loadfile(path)
	if f and type(f) == "function" then
		return f()
	end
end

function M:init(lang)
	local l = loadlang("assets/i18n/en-US.lua")
	self._language = type(l) == "table" and l or {}
	if lang and lang ~= "en-US" then
		local o = loadlang("assets/i18n/" .. lang .. ".lua")
		if type(o) == "table" then
			for k, v in pairs(o) do
				if v then
					self._language[k] = v
				end
			end
		end
	end
end

function M:__call(text)
	return self._language[text] or text
end

return M
