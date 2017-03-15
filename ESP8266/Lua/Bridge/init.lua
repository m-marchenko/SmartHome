--init.lua
print("set up wifi mode")
wifi.setmode(wifi.STATION)
wifi.sta.config("WatchOut!","djctvmgjgeufq") 
wifi.sta.connect()
tmr.alarm(1, 1000, 1, function() 
    if wifi.sta.getip()== nil then 
        print("IP unavaiable, Waiting...") 
    else 
        tmr.stop(1)
        print("Config done, IP is "..wifi.sta.getip())
        local pinwifi=2 --gpio 4
        gpio.mode(pinwifi, gpio.OUTPUT)
        gpio.write(pinwifi, gpio.LOW)
        tmr.delay(5000000)
        gpio.write(pinwifi, gpio.HIGH)
        --dofile("barrel.lua")
        dofile("bridge.lua")
    end 
 end)

