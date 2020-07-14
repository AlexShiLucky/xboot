local M = Class(DisplayObject)

function M:init(text, color, family, size)
	self._text = Text.new(text or "", color, family, size)
	local width, height = self._text:getSize()
	self.super:init(width, height, self._text)
end

function M:setWidth(width)
	return self
end

function M:setHeight(height)
	return self
end

function M:setSize(width, height)
	return self
end

function M:setText(text)
	self._text:setText(text or "")
	self.super:setSize(self._text:getSize())
	self:markDirty()
	return self
end

function M:setColor(color)
	self._text:setColor(color)
	self:markDirty()
	return self
end

function M:setFontFamily(family)
	if font then
		self._text:setFontFamily(family)
		self.super:setSize(self._text:getSize())
		self:markDirty()
	end
	return self
end

function M:setFontSize(size)
	self._text:setFontSize(size)
	self.super:setSize(self._text:getSize())
	self:markDirty()
	return self
end

return M
