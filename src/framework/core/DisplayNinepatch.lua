local M = Class(DisplayObject)

function M:init(ninepatch)
	if ninepatch then
		local w, h = ninepatch:getSize()
		self._ninepatch = ninepatch
		self.super:init(w, h, ninepatch)
	else
		self.super:init()
	end
end

function M:setWidth(width)
	if self._ninepatch then
		self._ninepatch:setWidth(width)
	end
	self.super:setWidth(width)
	return self
end

function M:setHeight(height)
	if self._ninepatch then
		self._ninepatch:setHeight(height)
	end
	self.super:setHeight(height)
	return self
end

function M:setSize(width, height)
	if self._ninepatch then
		self._ninepatch:setSize(width, height)
	end
	self.super:setSize(width, height)
	return self
end

return M
