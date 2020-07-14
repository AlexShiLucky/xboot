local table = table
local M = Class()

function M:init()
	self._elms = {}
end

function M:hasEventListener(type, listener, data)
	local data = data or self
	local elm = self._elms[type]

	if not elm or #elm == 0 then
		return false
	end

	for i, v in ipairs(elm) do
		if v.listener == listener and v.data == data then
			return true
		end
	end

	return false
end

function M:addEventListener(type, listener, data)
	local data = data or self

	if self:hasEventListener(type, listener, data) then
		return self
	end

	if not self._elms[type] then
		self._elms[type] = {}
	end

	local elm = self._elms[type]
	local el = {type = type, listener = listener, data = data}
	table.insert(elm, el)

	return self
end

function M:removeEventListener(type, listener, data)
	local data = data or self
	local elm = self._elms[type]

	if not elm or #elm == 0 then
		return self
	end

	for i, v in ipairs(elm) do
		if v.type == type and v.listener == listener and v.data == data then
			table.remove(elm, i)
			break
		end
	end

	return self
end

function M:dispatchEvent(event)
	if event.stop then
		return self
	end

	local elm = self._elms[event.type]

	if not elm or #elm == 0 then
		return self
	end

	for i, v in ipairs(elm) do
		if v.type == event.type then
			v.listener(v.data, event)
		end
	end

	return self
end

return M
