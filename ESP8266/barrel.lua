require('httpclient')
local pinstop=4 -- GPIO2
local pintop = 6 --GPIO12
local pinbottom = 7 --GPIO13
gpio.mode(pinstop, gpio.INPUT)
gpio.mode(pintop, gpio.OUTPUT)
gpio.mode(pinbottom, gpio.OUTPUT)
gpio.write(pintop, gpio.LOW)
gpio.write(pinbottom, gpio.LOW)

tmr.alarm(0,5000, 1, function()
if gpio.read(pinstop)==0 then 
tmr.stop(1) 
tmr.stop(2)
uart.on("data") 
print("Timers stopped") 
end end)

local host = "smart.no-troubles.com"
tmr.alarm(1, 15000, tmr.ALARM_AUTO, function()
 data = {target="barrel1_parnik1"}
 req = {}
 req.host = host
 req.method = "POST"
 req.webmethod = "/Command/RecieveCommand"
 --req.data = "{\"target\":\"barrel1_parnik1\"}"
 req.data = cjson.encode(data) 
 wsend(req, function(con, pl) 
    cmd = string.sub(pl,string.find(pl,"\r\n\r\n") + 4)    
    if cmd == "empty" then
       gpio.write(pintop, gpio.LOW)
       gpio.write(pinbottom, gpio.HIGH)
        print ("empty command")
    elseif cmd == "fill" then
       gpio.write(pintop, gpio.HIGH)
       gpio.write(pinbottom, gpio.LOW)    
       print("fill command")
    end
 end)
end)
tmr.alarm(2, 10000, tmr.ALARM_AUTO, function()
 data = {sensorId="t_parnik1", val=25}
 req = {}
 req.host = host
 req.method = "POST"
 req.webmethod = "/Command/SetSensorValue"
 --req.data = "{\"target\":\"barrel1_parnik1\"}"
 req.data = cjson.encode(data) 
 wsend(req, function(con, pl) 
    cmd = string.sub(pl,string.find(pl,"\r\n\r\n") + 4)    
    print("response:"..cmd)
 end)
 
end)
