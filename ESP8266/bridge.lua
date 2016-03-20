require('httpclient')
local pin=4 -- GPIO2
gpio.mode(pin, gpio.INPUT)

tmr.alarm(1,5000, 1, function()
if gpio.read(pin)==0 then 
tmr.stop(1) 
uart.on("data") 
print("Bridge stopped") 
end end)

local host = "smart.no-troubles.com"
tmr.alarm(2, 10000, tmr.ALARM_AUTO, function()
 data = {target="barrel1_parnik1"}
 req = {}
 req.host = host
 req.method = "POST"
 req.webmethod = "/Command/RecieveCommand"
 --req.data = "{\"target\":\"barrel1_parnik1\"}"
 req.data = cjson.encode(data) 
 wsend(req)
end)

http = {}
http.method="POST"
http.host=host
http.webmethod="/Command/SetSensorValue"

uart.on("data",0,function(data) 
--print(data) 
http.data=data
wsend(http) 
end, 0)
