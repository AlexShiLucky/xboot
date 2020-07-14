return function(o)
	if type(o) == "table" then
		local cache = {[o] = "."}
		local function dump(t, space, name)
			local temp = {}
			for k, v in pairs(t) do
				local key = tostring(k)
				if cache[v] then
					table.insert(temp, "+" .. key .. " {" .. cache[v] .. "}")
				elseif type(v) == "table" then
					local nkey = name .. "." .. key
					cache[v] = nkey
					table.insert(temp, "+" .. key .. dump(v, space .. (next(t, k) and "|" or " " ) .. string.rep(" ", #key), nkey))
				else
					table.insert(temp, "+" .. key .. " [" .. tostring(v) .. "]")
				end
			end
			return table.concat(temp, "\r\n" .. space)
		end
		print(dump(o, "", ""))
	else
		print(o)
	end
end
