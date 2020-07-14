local M = Class()

function M:init(delay, iteration, listener)
	self._delay = delay or 1
	self._iteration = iteration or 1
	self._listener = listener
	self._running = false
	self._runtime = 0
	self._runcount = 0
end

function M:start()
	self._running = true
end

function M:pause()
	self._running = false
end

function M:status()
	return self._running
end

return M
