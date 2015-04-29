
local json = require("dkjson")
math = require("math")

math.randomseed(os.time())

response = function(status, headers, body)
    if status == 200 then
        --print(body)
        --print(body)
    else
        --print(status .. " " .. body .. "\n")
    end
    if status == 500 then
        print(body)
    end
end

request = function()
    local num = tostring(math.random(0, 1000))
    wrk.scheme = "http"
    wrk.headers["Accept"] = "*/*"
    wrk.headers["User-Agent"] = "HTTPie/0.8.0"
    wrk.path = "/" .. num .. "/"
    wrk.method = "GET"
    r = wrk.format()
    return r
end
