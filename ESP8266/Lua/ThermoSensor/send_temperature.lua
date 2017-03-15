t = require("ds18b20")

-- ESP-01 GPIO Mapping
gpio2 = 4

t.setup(gpio2)
addrs = t.addrs()
if (addrs ~= nil) then
  print("Total DS18B20 sensors: "..table.getn(addrs).."\n")
  print(string.len(addrs[1]).."\n")
  addr = ""
for i=1,string.len(addrs[1]) do
addr = addr .. string.format('%02x', addrs[1]:byte(i))
end
   print (addr) 
end
r = t.read()
r = t.read()
-- Just read temperature
print("Temperature: "..r.."'C")

-- Don't forget to release it after use
t = nil
ds18b20 = nil
package.loaded["ds18b20"]=nil

require('httpclient')
local host = "smart.no-troubles.com"

http = {}
http.method="POST"
http.host=host
http.webmethod="/Command/SetSensorValue"
http.data = "{\"sensorId\":\"TM0002\", \"val\":\""..r.."\"}"
wsend(http, function(con, pl) print(pl) end) 

