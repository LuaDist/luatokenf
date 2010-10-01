-- debug filter

local function IN(get,put)
	put("IN init")
	while true do
		local line,token,value=get()
		print(line,token,value)
		put(line,token,value)
	end
end

local function OUT(get,put)
	put("OUT init")
	while true do
		local line,token,value=get()
		print("",line,token,value)
		put(line,token,value)
	end
end

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

local function pipe(f,get,put)
	put = put or coroutine.yield
	local F=coroutine.wrap(f)
	F(get,put)
	return F
end

function FILTER(get,source)
	FILTER=pipe(OUT,pipe(F,pipe(IN,get)))
end
