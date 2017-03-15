--init.lua - client
print("set up wifi mode")
wifi.setmode(wifi.STATION)
wifi.sta.config("NodeMcuRoot","A1234567890") 
--wifi.sta.config("MegaFon_MR100-1","95955870") 
wifi.sta.connect()
i=0

ipcfg = 
{
  ip = "192.168.20.101",
  netmask = "255.255.255.0",
  gateway = "192.168.20.101"
}

isconnected = false

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

        local conn=net.createConnection(net.TCP, false) 
        conn:on("connection", function(cn, data) 
            print("Connected...") 
            --isconnected=true 
            --msg="message #"..tostring(i)
            --print("sending message: "..msg)                        
            --conn:send(msg)
            --i=i+1            
            end)
        
        conn:on("disconnection", function(cn, data) 
            print("Disconnected...") 
            isconnected=false 
            conn:connect(80, "192.168.20.10") 
            end)
        
        conn:on("receive", function(cn, pl) print(pl) end)        
        
        --conn:on("sent", function(cn, data) print(data) end)

        conn:connect(80, "192.168.20.10")
        
        tmr.alarm(0, 3000, 1, function() 
           --if isconnected then
            msg="message #"..tostring(i)
            print("sending message: "..msg)
            conn:send(msg) 
            i=i+1
            --end
        end)

        
    end 
 end)

--r = wifi.sta.setip(ipcfg)
--print(r)

