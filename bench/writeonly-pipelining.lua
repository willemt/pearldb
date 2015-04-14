
local json = require("dkjson")
math = require("math")

pipeline_depth = 16

math.randomseed(os.time())

request = function()
    local r = {}
    wrk.scheme = "http"
    wrk.headers["Accept"] = "*/*"
    wrk.headers["User-Agent"] = "HTTPie/0.8.0"
    wrk.method = "PUT"

    for i=1,pipeline_depth do
        local num = tostring(math.random(0, 1000))
        tbl = {
           x = "" .. num,
        }

        wrk.body = json.encode(tbl, { indent = true })
        r[i] = wrk.format(nil, "/" .. tostring(num) .. "/")
    end

    return table.concat(r)
end
