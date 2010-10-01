-- sugar "in <name> do" -> "in next,<name> do"
local function F(get,put)
	put("F init")
	while true do
		local line1,token1,value1=get()
		put(line1,token1,value1)
		if token1=="in" then
			local line2,token2,value2=get()
			local line3,token3,value3=get()
			if token2=="<name>" and token3=="do" then
				put(line2,"<name>","next")
				put(line2,",")
			end
			put(line2,token2,value2)
			put(line3,token3,value3)
		end
	end
end

function FILTER(get,source)
	FILTER=coroutine.wrap(F)
	FILTER(get,coroutine.yield)
end
