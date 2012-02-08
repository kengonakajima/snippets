a = Range.new( 1,2, TRUE )

print a, "\n"

a = Range.new( 1,2, FALSE )

print a, "\n"

a = 1...2

print a.exclude_end?, "\n"
