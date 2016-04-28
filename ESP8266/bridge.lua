require('httpclient')
function mqttconnect(client)
client:connect("m10.cloudmqtt.com", 16425, 0, 
  function(client)
   client:subscribe("root/parnik1/#",0, function(client)  end)
   gpio.write(2, gpio.LOW)
  end, 
  function(client, reason) print("failed reason: "..reason) end)
end

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
client:on("connect", function(client)  end)
client:on("message", function(client, topic, data)  
  if data ~= nil then
    print(data.."\r")
  end
end)

client:on("offline", function(client) 
gpio.write(2, gpio.HIGH)
mqttconnect(client) 
end)

mqttconnect(client)

uart.on("data", "\r",function(data) 
ok, json = pcall(cjson.decode, data)
if ok then
--print(data) 
--client:publish("sensor", data, 0, 0)
http.data=data
wsend(http, function(con, pl) end) 
else
client:publish("error", data, 0, 0)
end
end, 0)

