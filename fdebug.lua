-- debug filter
function FILTER(get,source)
	print("FILTER",source)
	FILTER=function ()
		local line,token,value=get()
		print("FILTER",line,token,value)
		return line,token,value
	end
end
