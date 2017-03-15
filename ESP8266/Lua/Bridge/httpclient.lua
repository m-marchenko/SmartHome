function wsend(request, onrecieve)
result = nil
conn=net.createConnection(net.TCP, false) 
conn:on("receive", onrecieve)
--conn:on("disconnection", function(conn) print("Got disconnection...") end)
conn:on("connection", function(conn, payload) 
--print('\nConnected') 
r = request.method.." http://"..request.host..request.webmethod.." HTTP/1.1\r\n" 
r = r.."Host:"..request.host.."\r\n"
r = r.."Accept: */*\r\n"
r = r.."User-Agent: Mozilla/4.0 (compatible; esp8266 Lua)\r\n"
if request.data ~= nil then
r = r.."Content-Type: application/json\r\n"
r = r.."Content-Length:"..#request.data.."\r\n\r\n"
r = r..request.data

end
--print(r)
conn:send(r)

end)                                
conn:connect(80,request.host)

end
