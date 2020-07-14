local M = Class(DisplayObject)

function M:init(image)
	if image then
		local w, h = image:getSize()
		self._image = image
		self.super:init(w, h, image)
	else
		self.super:init()
	end
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

function M:getImage()
	return self._image
end

return M
