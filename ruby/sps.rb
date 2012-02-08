#!/usr/local/bin/ruby

require 'rubygems'
require 'spreadsheet'

Spreadsheet.client_encoding = 'UTF-8'

book = Spreadsheet::Workbook.new
worksheet = book.create_worksheet
worksheet.name = "myname"

worksheet.column(0).width = 5
worksheet.column(1).width = 10

default_format = worksheet.default_format
default_format.horizontal_align = :right

cell_red = default_format.clone
cell_red.pattern = 1
cell_red.pattern_fg_color = :red

worksheet[0, 0] = 0
worksheet[1, 1] = 1
worksheet.row(1).set_format(1, cell_red)

xls_filename = "test.xls"
book.write(xls_filename)
