local json = require("dkjson")
math = require("math")

pipeline_depth = 16

math.randomseed(os.time())

request = function()
    local r = {}
    wrk.scheme = "http"
    wrk.headers["Accept"] = "*/*"
    wrk.headers["User-Agent"] = "HTTPie/0.8.0"
    wrk.method = "GET"

    for i=1,pipeline_depth do
        r[i] = wrk.format(nil, "/" .. tostring(math.random(1, 10000)) .. "/")
    end

    return table.concat(r)
end
