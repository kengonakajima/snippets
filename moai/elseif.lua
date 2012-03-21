local st = os.clock()
g =0 
for i=1,100000 do
  local t = i % 100
  if t==0 then
    g = g + 0
  elseif t == 1 then
    g = g + 1
  elseif t == 2 then
    g = g + 2
  elseif t == 3 then
    g = g + 3
  elseif t == 4 then
    g = g + 4
  elseif t == 5 then
    g = g + 5
  elseif t == 6 then
    g = g + 6
  elseif t == 7 then
    g = g + 7
  elseif t == 8 then
    g = g + 8
  elseif t == 9 then
    g = g + 9
  elseif t == 10 then
    g = g + 10
  elseif t == 11 then
    g = g + 11
  elseif t == 12 then
    g = g + 12
  elseif t == 13 then
    g = g + 13
  elseif t == 14 then
    g = g + 14
  elseif t == 15 then
    g = g + 15
  elseif t == 16 then
    g = g + 16
  elseif t == 17 then
    g = g + 17
  elseif t == 18 then
    g = g + 18
  elseif t == 19 then
    g = g + 19
  elseif t == 20 then
    g = g + 20
  elseif t == 21 then
    g = g + 21
  elseif t == 22 then
    g = g + 22
  elseif t == 23 then
    g = g + 23
  elseif t == 24 then
    g = g + 24
  elseif t == 25 then
    g = g + 25
  elseif t == 26 then
    g = g + 26
  elseif t == 27 then
    g = g + 27
  elseif t == 28 then
    g = g + 28
  elseif t == 29 then
    g = g + 29
  elseif t == 30 then
    g = g + 30
  elseif t == 31 then
    g = g + 31
  elseif t == 32 then
    g = g + 32
  elseif t == 33 then
    g = g + 33
  elseif t == 34 then
    g = g + 34
  elseif t == 35 then
    g = g + 35
  elseif t == 36 then
    g = g + 36
  elseif t == 37 then
    g = g + 37
  elseif t == 38 then
    g = g + 38
  elseif t == 39 then
    g = g + 39
  elseif t == 40 then
    g = g + 40
  elseif t == 41 then
    g = g + 41
  elseif t == 42 then
    g = g + 42
  elseif t == 43 then
    g = g + 43
  elseif t == 44 then
    g = g + 44
  elseif t == 45 then
    g = g + 45
  elseif t == 46 then
    g = g + 46
  elseif t == 47 then
    g = g + 47
  elseif t == 48 then
    g = g + 48
  elseif t == 49 then
    g = g + 49
  elseif t == 50 then
    g = g + 50
  elseif t == 51 then
    g = g + 51
  elseif t == 52 then
    g = g + 52
  elseif t == 53 then
    g = g + 53
  elseif t == 54 then
    g = g + 54
  elseif t == 55 then
    g = g + 55
  elseif t == 56 then
    g = g + 56
  elseif t == 57 then
    g = g + 57
  elseif t == 58 then
    g = g + 58
  elseif t == 59 then
    g = g + 59
  elseif t == 60 then
    g = g + 60
  elseif t == 61 then
    g = g + 61
  elseif t == 62 then
    g = g + 62
  elseif t == 63 then
    g = g + 63
  elseif t == 64 then
    g = g + 64
  elseif t == 65 then
    g = g + 65
  elseif t == 66 then
    g = g + 66
  elseif t == 67 then
    g = g + 67
  elseif t == 68 then
    g = g + 68
  elseif t == 69 then
    g = g + 69
  elseif t == 70 then
    g = g + 70
  elseif t == 71 then
    g = g + 71
  elseif t == 72 then
    g = g + 72
  elseif t == 73 then
    g = g + 73
  elseif t == 74 then
    g = g + 74
  elseif t == 75 then
    g = g + 75
  elseif t == 76 then
    g = g + 76
  elseif t == 77 then
    g = g + 77
  elseif t == 78 then
    g = g + 78
  elseif t == 79 then
    g = g + 79
  elseif t == 80 then
    g = g + 80
  elseif t == 81 then
    g = g + 81
  elseif t == 82 then
    g = g + 82
  elseif t == 83 then
    g = g + 83
  elseif t == 84 then
    g = g + 84
  elseif t == 85 then
    g = g + 85
  elseif t == 86 then
    g = g + 86
  elseif t == 87 then
    g = g + 87
  elseif t == 88 then
    g = g + 88
  elseif t == 89 then
    g = g + 89
  elseif t == 90 then
    g = g + 90
  elseif t == 91 then
    g = g + 91
  elseif t == 92 then
    g = g + 92
  elseif t == 93 then
    g = g + 93
  elseif t == 94 then
    g = g + 94
  elseif t == 95 then
    g = g + 95
  elseif t == 96 then
    g = g + 96
  elseif t == 97 then
    g = g + 97
  elseif t == 98 then
    g = g + 98
  elseif t == 99 then
    g = g + 99
  elseif t == 100 then
    g = g + 100
  end
end

local et = os.clock()
print(100, g, (et-st))

function f1(t)
  g = g + t
end
function f2(t)
  g = g + t
end
function f3(t)
  g = g + t
end
function f4(t)
  g = g + t
end
function f5(t)
  g = g + t
end
function f6(t)
  g = g + t
end
function f7(t)
  g = g + t
end
function f8(t)
  g = g + t
end
function f9(t)
  g = g + t
end
function f10(t)
  g = g + t
end
function f11(t)
  g = g + t
end
function f12(t)
  g = g + t
end
function f13(t)
  g = g + t
end
function f14(t)
  g = g + t
end
function f15(t)
  g = g + t
end
function f16(t)
  g = g + t
end
function f17(t)
  g = g + t
end
function f18(t)
  g = g + t
end
function f19(t)
  g = g + t
end
function f20(t)
  g = g + t
end
function f21(t)
  g = g + t
end
function f22(t)
  g = g + t
end
function f23(t)
  g = g + t
end
function f24(t)
  g = g + t
end
function f25(t)
  g = g + t
end
function f26(t)
  g = g + t
end
function f27(t)
  g = g + t
end
function f28(t)
  g = g + t
end
function f29(t)
  g = g + t
end
function f30(t)
  g = g + t
end
function f31(t)
  g = g + t
end
function f32(t)
  g = g + t
end
function f33(t)
  g = g + t
end
function f34(t)
  g = g + t
end
function f35(t)
  g = g + t
end
function f36(t)
  g = g + t
end
function f37(t)
  g = g + t
end
function f38(t)
  g = g + t
end
function f39(t)
  g = g + t
end
function f40(t)
  g = g + t
end
function f41(t)
  g = g + t
end
function f42(t)
  g = g + t
end
function f43(t)
  g = g + t
end
function f44(t)
  g = g + t
end
function f45(t)
  g = g + t
end
function f46(t)
  g = g + t
end
function f47(t)
  g = g + t
end
function f48(t)
  g = g + t
end
function f49(t)
  g = g + t
end
function f50(t)
  g = g + t
end
function f51(t)
  g = g + t
end
function f52(t)
  g = g + t
end
function f53(t)
  g = g + t
end
function f54(t)
  g = g + t
end
function f55(t)
  g = g + t
end
function f56(t)
  g = g + t
end
function f57(t)
  g = g + t
end
function f58(t)
  g = g + t
end
function f59(t)
  g = g + t
end
function f60(t)
  g = g + t
end
function f61(t)
  g = g + t
end
function f62(t)
  g = g + t
end
function f63(t)
  g = g + t
end
function f64(t)
  g = g + t
end
function f65(t)
  g = g + t
end
function f66(t)
  g = g + t
end
function f67(t)
  g = g + t
end
function f68(t)
  g = g + t
end
function f69(t)
  g = g + t
end
function f70(t)
  g = g + t
end
function f71(t)
  g = g + t
end
function f72(t)
  g = g + t
end
function f73(t)
  g = g + t
end
function f74(t)
  g = g + t
end
function f75(t)
  g = g + t
end
function f76(t)
  g = g + t
end
function f77(t)
  g = g + t
end
function f78(t)
  g = g + t
end
function f79(t)
  g = g + t
end
function f80(t)
  g = g + t
end
function f81(t)
  g = g + t
end
function f82(t)
  g = g + t
end
function f83(t)
  g = g + t
end
function f84(t)
  g = g + t
end
function f85(t)
  g = g + t
end
function f86(t)
  g = g + t
end
function f87(t)
  g = g + t
end
function f88(t)
  g = g + t
end
function f89(t)
  g = g + t
end
function f90(t)
  g = g + t
end
function f91(t)
  g = g + t
end
function f92(t)
  g = g + t
end
function f93(t)
  g = g + t
end
function f94(t)
  g = g + t
end
function f95(t)
  g = g + t
end
function f96(t)
  g = g + t
end
function f97(t)
  g = g + t
end
function f98(t)
  g = g + t
end
function f99(t)
  g = g + t
end
function f100(t)
  g = g + t
end

funcs={}

funcs[1]=f1
funcs[2]=f2
funcs[3]=f3
funcs[4]=f4
funcs[5]=f5
funcs[6]=f6
funcs[7]=f7
funcs[8]=f8
funcs[9]=f9
funcs[10]=f10
funcs[11]=f11
funcs[12]=f12
funcs[13]=f13
funcs[14]=f14
funcs[15]=f15
funcs[16]=f16
funcs[17]=f17
funcs[18]=f18
funcs[19]=f19
funcs[20]=f20
funcs[21]=f21
funcs[22]=f22
funcs[23]=f23
funcs[24]=f24
funcs[25]=f25
funcs[26]=f26
funcs[27]=f27
funcs[28]=f28
funcs[29]=f29
funcs[30]=f30
funcs[31]=f31
funcs[32]=f32
funcs[33]=f33
funcs[34]=f34
funcs[35]=f35
funcs[36]=f36
funcs[37]=f37
funcs[38]=f38
funcs[39]=f39
funcs[40]=f40
funcs[41]=f41
funcs[42]=f42
funcs[43]=f43
funcs[44]=f44
funcs[45]=f45
funcs[46]=f46
funcs[47]=f47
funcs[48]=f48
funcs[49]=f49
funcs[50]=f50
funcs[51]=f51
funcs[52]=f52
funcs[53]=f53
funcs[54]=f54
funcs[55]=f55
funcs[56]=f56
funcs[57]=f57
funcs[58]=f58
funcs[59]=f59
funcs[60]=f60
funcs[61]=f61
funcs[62]=f62
funcs[63]=f63
funcs[64]=f64
funcs[65]=f65
funcs[66]=f66
funcs[67]=f67
funcs[68]=f68
funcs[69]=f69
funcs[70]=f70
funcs[71]=f71
funcs[72]=f72
funcs[73]=f73
funcs[74]=f74
funcs[75]=f75
funcs[76]=f76
funcs[77]=f77
funcs[78]=f78
funcs[79]=f79
funcs[80]=f80
funcs[81]=f81
funcs[82]=f82
funcs[83]=f83
funcs[84]=f84
funcs[85]=f85
funcs[86]=f86
funcs[87]=f87
funcs[88]=f88
funcs[89]=f89
funcs[90]=f90
funcs[91]=f91
funcs[92]=f92
funcs[93]=f93
funcs[94]=f94
funcs[95]=f95
funcs[96]=f96
funcs[97]=f97
funcs[98]=f98
funcs[99]=f99
funcs[100]=f100

st = os.clock()
g = 0

for i=1,100000 do
  local t = (i % 100)+1
  local f = funcs[t]
  f(t)
end

et = os.clock()
print(100, g,(et-st))

st = os.clock()
g=0
for i=1,100000 do
  local t = (i % 10)
  if t==0 then
    g = g + 0
  elseif t == 1 then
    g = g + 1
  elseif t == 2 then
    g = g + 2
  elseif t == 3 then
    g = g + 3
  elseif t == 4 then
    g = g + 4
  elseif t == 5 then
    g = g + 5
  elseif t == 6 then
    g = g + 6
  elseif t == 7 then
    g = g + 7
  elseif t == 8 then
    g = g + 8
  elseif t == 9 then
    g = g + 9
  elseif t == 10 then
    g = g + 10
  end
end
et = os.clock()
print(10,g,(et-st))

st = os.clock()
g=0
for i=1,100000 do
  local t = (i%10)+1
  local f = funcs[t]
  f(t)
end
et = os.clock()
print(10,g,(et-st))
