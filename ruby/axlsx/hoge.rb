#
#
require "axlsx"


package = Axlsx::Package.new                             
sheet = package.workbook.add_worksheet(name: 'curry')    

item_style = package.workbook.styles.add_style :b => false, :sz => 9,  :font_name => 'MS Pゴシック',
  :alignment => { :horizontal => :left, :vertical => :center, :wrap_text => true}

sheet.add_row(['品名', '単価', '数量', '計'])            
sheet.add_row(['にんじん',    80, 1,      '=B2*C2'], :style => item_style )     
sheet.add_row(['たまねぎ',    50, 2,      '=B3*C3'])     
sheet.add_row(['じゃがいも',  40, 2,      '=B4*C4'])     
sheet.add_row(['牛肉',       200, 1,      '=B5*C5'])     
sheet.add_row(['カレー粉',   150, 1,      '=B6*C6'])     
sheet.add_row(['',            '', '総計', '=SUM(D2:D6)'])


sheet.add_chart(Axlsx::Pie3DChart, :start_at => [0,2], :end_at => [5, 15], :title=> 'dark corner here') do |chart|      
  chart.add_series :data => sheet["D2:D6"], :labels => sheet["A2:A6"]                                                   
  chart.d_lbls.show_val = true                                                                                          
  chart.d_lbls.show_percent = true                                                                                      
  chart.d_lbls.d_lbl_pos = :outEnd                                                                                      
  chart.d_lbls.show_leader_lines = true                                                                                 
  chart.start_at 10,6
  chart.end_at 10+5,6+10
end   

                                                                                                                          
# line chart                                                                                                              
sheet.add_chart(Axlsx::LineChart,  :title => "Chart") do |chart|               
  chart.add_series :data => sheet["D2:D6"], :labels => sheet["A2:A6"], :title => 'bob', :smooth=>false, :show_marker=>true
  chart.d_lbls.show_val = true                                                                                          
  chart.d_lbls.show_cat_name = true                                                                                     
#  chart.catAxis.tick_lbl_pos = :none                                                                                    
  chart.start_at 10,6+12
  chart.end_at 10+8,6+15+12
end      


sheet.add_chart(Axlsx::Bar3DChart) do |chart|                                      
  chart.add_series :data => sheet["D2:D6"], :labels => sheet["A2:A6"], :title => "zzz" #sheet["A1"]                            
  chart.valAxis.label_rotation = -45                                                                                    
  chart.catAxis.label_rotation = 45                                                                                     
  chart.d_lbls.d_lbl_pos = :outEnd                                                                                      
  chart.d_lbls.show_val = true                                                                                          
  chart.catAxis.tick_lbl_pos = :none                                                                                    
  chart.start_at 10,6+25
  chart.end_at 10+8,6+15+25
  chart.bar_dir = :col  #:bar
end             

sheet.add_chart(Axlsx::Bar3DChart) do |chart|                                      
  chart.add_series :data => sheet["D2:D6"], :labels => sheet["A2:A6"], :colors => ["88f700","88f700","88f700","777777","777777","777777"], :title => "zzz" #sheet["A1"]                            
  chart.valAxis.label_rotation = -45                                                                                    
  chart.catAxis.label_rotation = 45                                                                                     
  chart.d_lbls.d_lbl_pos = :outEnd                                                                                      
  chart.d_lbls.show_val = true                                                                                          
  chart.catAxis.tick_lbl_pos = :none                                                                                    
  chart.start_at 0,8
  chart.end_at 8,8+15
  chart.bar_dir = :col  #:bar
end             



package.serialize('test.xlsx')    
