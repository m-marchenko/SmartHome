--init.lua - AP+server
print("set up wifi AP+station mode")
wifi.setmode(wifi.STATIONAP)

print ("configuring access point")

cfg={}
cfg.ssid = "NodeMcuRoot"
cfg.pwd = "A1234567890"
--cfg.auth = wifi.WPA_PSK
--cfg.max = 4

wifi.ap.config(cfg)
ipcfg =
{
    ip="192.168.20.10",
    netmask="255.255.255.0",
    gateway="192.168.20.10"
}
r = wifi.ap.setip(ipcfg)

--dhcp_config ={}
--dhcp_config.start = "201.105.10.100"
--wifi.ap.dhcp.config(dhcp_config)
--wifi.ap.dhcp.start()

--wifi.eventmon.register(wifi.eventmon.AP_STACONNECTED, function(T)
-- print("\n\tAP - STATION CONNECTED".."\n\tMAC: "..T.MAC.."\n\tAID: "..T.AID)
-- end)
 
--wifi.eventmon.register(wifi.eventmon.AP_STADISCONNECTED, function(T)
-- print("\n\tAP - STATION DISCONNECTED".."\n\tMAC: "..T.MAC.."\n\tAID: "..T.AID)
-- end) 

tmr.alarm(0, 10000, 1, function() 
    for mac,ip in pairs(wifi.ap.getclient()) do
        print(mac,ip)
    end
 end)

print("Access point configuration done. IP is "..wifi.ap.getip())    

print ("configuring station")
wifi.sta.config("WatchOut!","djctvmgjgeufq") 
wifi.sta.connect()
tmr.alarm(1, 1000, 1, function() 
    if wifi.sta.getip()== nil then 
        print("IP unavaiable, Waiting...") 
    else 
        tmr.stop(1)
        print("Config done, IP is "..wifi.sta.getip())
        --local pinwifi=2 --gpio 4
        --gpio.mode(pinwifi, gpio.OUTPUT)
        --gpio.write(pinwifi, gpio.LOW)
        --tmr.delay(5000000)
        --gpio.write(pinwifi, gpio.HIGH)
        --dofile("barrel.lua")
        --dofile("bridge.lua")
    end 
 end)
 
print("Creating TCP server")
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
  conn:on("receive",function(conn,payload)
    print(payload)
    conn:send("Echo:"..payload)
  end)
  --conn:on("sent",function(conn) conn:close() end)
end)
