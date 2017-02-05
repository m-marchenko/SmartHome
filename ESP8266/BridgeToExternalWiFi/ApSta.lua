--init.lua
print("set up wifi AP+station mode")
wifi.setmode(wifi.STATIONAP)

print ("configuring access point")

cfg={}
cfg.ssid = "NodeMcuRoot"
cfg.pwd = "A1234567890"
--cfg.auth = wifi.WPA_PSK

wifi.ap.config(cfg)
ipcfg =
{
    ip="192.168.4.1",
    netmask="255.255.255.0",
    gateway="192.168.4.1"
}
r = wifi.ap.setip(cfg)

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

