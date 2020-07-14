local Xfs = Xfs
local Font = Font
local Image = Image
local Ninepatch = Ninepatch
local DisplayImage = DisplayImage
local DisplayNinepatch = DisplayNinepatch

local M = Class()

function M:init()
	self._images = {}
	self._themes = {}
end

function M:loadImage(name)
	if type(name) == "string" then
		if not self._images[name] and Xfs.isfile(name) then
			self._images[name] = Image.new(name)
		end
		return self._images[name]
	end
	return nil
end

function M:loadTheme(name)
	local default = "assets/themes/default"
	local name = type(name) == "string" and name or default
	if not self._themes[name] then
		if Xfs.isdir(name) then
			self._themes[name] = require(name)
		end
		if not self._themes[name] then
			name = "assets/themes/" .. name
			if Xfs.isdir(name) then
				self._themes[name] = require(name)
			end
			if not self._themes[name] then
				if not self._themes[default] then
					if Xfs.isdir(default) then
						self._themes[default] = require(default)
					end
				end
				return self._themes[default]
			end
		end
	end
	return self._themes[name]
end

function M:loadDisplay(name)
	if type(name) == "string" and Xfs.isfile(name) then
		if string.lower(string.sub(name, -6)) == ".9.png" then
			return DisplayNinepatch.new(Ninepatch.new(name))
		elseif string.lower(string.sub(name, -4)) == ".png" then
			return DisplayImage.new(self:loadImage(name))
		elseif string.lower(string.sub(name, -4)) == ".jpg" then
			return DisplayImage.new(self:loadImage(name))
		elseif string.lower(string.sub(name, -5)) == ".jpeg" then
			return DisplayImage.new(self:loadImage(name))
		end
	else
		return name
	end
end

function M:clear()
	self._images = {}
	self._themes = {}
end

return M
