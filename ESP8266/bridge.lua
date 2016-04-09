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

http = {}
http.method="POST"
http.host=host
http.webmethod="/Command/SetSensorValue"

local client=mqtt.Client("parnik1", 6000, "quefkobd", "524rVRkbylHl")
client:on("connect", function(client) print ("connected") end)
client:on("message", function(client, topic, data) 
  print(topic .. ":" ) 
  if data ~= nil then
    print(data)
  end
end)
client:connect("m10.cloudmqtt.com", 16425, 0, 
  function(client) print("connected") 
   client:subscribe("root/parnik1/#",0, function(client) print("subscribe success") end)
  end, 
  function(client, reason) print("failed reason: "..reason) end)

uart.on("data",0,function(data) 
--print(data) 
client:publish("sensor", data, 0, 0)
http.data=data
wsend(http, function(con, pl) end) 
end, 0)
