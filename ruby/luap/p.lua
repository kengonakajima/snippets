-- Copyright github.com/kengonakajima 2012
-- License: Apache 2.0
-- Compatibility: server: luvit, client: MoaiSDK

-- envs
local luvit, moai = {}, {}

if not _G.lumino then
  _G.lumino = {}
end

pcall( function()
    _G.socket = require("socket")
  end)

if MOAISim then

else
  _G.table = require("table")
  _G.math = require("math")
  _G.io = require("io")
  _G.string = require("string")
  _G.os = require("os")
  _G.dbg = require("debug")
  local res,uv = pcall( function() return require("uv_native") end)
  if res and uv then -- luvit only
    _G.uv = uv
    _G.ffi = require("ffi")
    _G.net = require("net")
    _G.JSON = require("json")
    _G.http = require("http")
    _G.timer = require("timer")
    _G.fs = require("fs")
--    _G.utils = require("utils")
  end
end
