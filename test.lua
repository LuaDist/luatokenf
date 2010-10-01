local t = { 100, 200, 300 }

for k,v in t do
	print("A",k,v)
end

for k,v in pairs(t) do
	print("B",k,v)
end
