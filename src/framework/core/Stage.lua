local M = Class(DisplayObject)

function M:init()
	self._running = true
	self._timerlist = {}
	self._window = Window.new()
	self.super:init(self._window:getSize())
	self:markDirty()
end

function M:exit()
	self._running = false
	return self
end

function M:hasTimer(timer)
	local tl = self._timerlist

	if not tl or #tl == 0 then
		return false
	end

	for i, v in ipairs(tl) do
		if v == timer then
			return true
		end
	end

	return false
end

function M:addTimer(timer)
	if self:hasTimer(timer) then
		return false
	end

	timer:start()
	table.insert(self._timerlist, timer)
	return true
end

function M:removeTimer(timer)
	local tl = self._timerlist

	if not tl or #tl == 0 then
		return false
	end

	for i, v in ipairs(tl) do
		if v == timer then
			v:pause()
			table.remove(tl, i)
			return true
		end
	end

	return false
end

function M:schedTimer(dt)
	for i, v in ipairs(self._timerlist) do
		if v._running then
			v._runtime = v._runtime + dt

			if v._runtime >= v._delay then
				v._runcount = v._runcount + 1
				v._listener(v)
				v._runtime = 0

				if v._iteration ~= 0 and v._runcount >= v._iteration then
					self:removeTimer(v)
				end
			end
		end
	end
end

function M:getDotsPerInch()
	local w, h = self._window:getSize()
	local pw, ph = self._window:getPhysicalSize()
	return w * 25.4 / pw, h * 25.4 / ph
end

function M:setBacklight(brightness)
	return self._window:setBacklight(brightness)
end

function M:getBacklight()
	return self._window:getBacklight()
end

function M:snapshot()
	return self._window:snapshot()
end

function M:addFont(family, path)
	return self._window:addFont(family, path)
end

function M:loop()
	local Event = Event
	local window = self._window
	local stopwatch = Stopwatch.new()

	self:addTimer(Timer.new(1 / 60, 0, function(t)
		self:dispatch(Event.new("enter-frame"))
		self:render(window)
		collectgarbage("step")
	end))

	while self._running do
		local e = Event.pump()
		if e ~= nil then
			if e.type == "system-exit" then
				self:exit()
			end
			self:dispatch(e)
		end

		local elapsed = stopwatch:elapsed()
		if elapsed > 0 then
			stopwatch:reset()
			self:schedTimer(elapsed)
		end
	end
end

return M
