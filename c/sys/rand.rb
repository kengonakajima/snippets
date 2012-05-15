#!/usr/bin/ruby
srand( Time.now.to_i)

if rand() < 0.5 then
  exit 1
else
  exit 0
end
