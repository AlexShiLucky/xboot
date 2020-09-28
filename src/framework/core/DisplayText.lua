local M = Class(DisplayObject)

function M:init(text, color, wrap, family, size)
	self._text = Text.new(text or "", color, wrap, family, size)
	local x, y, w, h = self._text:getMetrics()
	self.super:init(w, h, self._text)
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
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

function M:setColor(color)
	self._text:setColor(color)
	self:markDirty()
	return self
end

function M:setWrap(wrap)
	self._text:setWrap(wrap)
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

function M:setFamily(family)
	self._text:setFamily(family)
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

function M:setSize(size)
	self._text:setSize(size)
	local x, y, w, h = self._text:getMetrics()
	self.super:setSize(w, h)
	self:markDirty()
	return self
end

return M
