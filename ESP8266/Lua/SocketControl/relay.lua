local pinrelay = 4 -- gpio2
gpio.mode(pinrelay, gpio.OUTPUT)
local level = gpio.HIGH
gpio.write(pinrelay, level)
tmr.alarm(1, 3000, 1, function() 
    if level == gpio.HIGH then
        level = gpio.LOW
    else
        level = gpio.HIGH
    end
    print("Writing level "..level)
    
    gpio.write(pinrelay, level)
end)