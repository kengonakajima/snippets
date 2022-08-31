require "nokogiri"

path=ARGV[0]
if !path then
  print "need path\n"
  exit 1
end

node_cnt=0
way_cnt=0
ref_cnt=0
relation_cnt=0

node_tag_keys={}
way_tag_keys={}
relation_tag_keys={}

File.open(path,"r") do |io|
  reader = Nokogiri::XML::Reader.from_io(io)
  reader.each do |node|
    if node.name=="node" then
      node_cnt+=1
      id=node.attributes["id"]
      lat=node.attributes["lat"]
      lng=node.attributes["lat"]      
      #print id,",",lat,",",lng,"\n"
      node.inner_xml.split("\n").each do |line|
        doc=Nokogiri::XML(line)
        doc.children.each do |nd|
          if nd.name=="tag" then 
            k=nd.attributes["k"]
            v=nd.attributes["v"]
#          print "TAG:",k,",",v,"\n"
            if !node_tag_keys[k.to_s] then
              print "new node tag key: #{k}\n"
              node_tag_keys[k.to_s]=0
            end          
            node_tag_keys[k.to_s]+=1
          end
        end       
      end
    elsif node.name=="way" then
      way_cnt+=1
      id=node.attributes["id"]
      node.inner_xml.split("\n").each do |line|
        doc=Nokogiri::XML(line)
        doc.children.each do |nd|
          if nd.name=="nd" then 
            ref=nd.attributes["ref"]
            ref_cnt+=1
#            print "REF:",ref,"way_id:",id,"\n"
          elsif nd.name=="tag" then
            k=nd.attributes["k"]
            v=nd.attributes["v"]
            if !way_tag_keys[k.to_s] then
              print "new way tag key: #{k}\n"
              way_tag_keys[k.to_s]=0
            end          
            way_tag_keys[k.to_s]+=1
          end          
        end
      end
    elsif node.name=="relation" then
      relation_cnt+=1
      id=node.attributes["id"]
      node.inner_xml.split("\n").each do |line|
        doc=Nokogiri::XML(line)
        doc.children.each do |nd|
          if nd.name=="member" then 
            type=nd.attributes["type"]
            ref=nd.attributes["ref"]
            ref_cnt+=1
#            print "REF:",ref,"way_id:",id,"\n"
          elsif nd.name=="tag" then
            k=nd.attributes["k"]
            v=nd.attributes["v"]
            if !relation_tag_keys[k.to_s] then
              print "new relation tag key: #{k}\n"
              relation_tag_keys[k.to_s]=0
            end          
            relation_tag_keys[k.to_s]+=1
          end          
        end
      end
    else
      next
    end    
    tot=node_cnt+way_cnt+ref_cnt
    if tot%100000==0 then
      print "total: #{tot} : #{node_cnt} nodes, #{way_cnt} ways, #{ref_cnt} refs\n"
    end    
  end
end

print "node tags:\n"
node_tag_keys.sort {|a,b| a[1]<=>b[1]} .each do |pair|
  print pair[1], " ", pair[0],"\n"
end
print "way tags:\n"
way_tag_keys.sort {|a,b| a[1]<=>b[1]} .each do |pair|
  print pair[1], " ", pair[0],"\n"
end
print "relation tags:\n"
relation_tag_keys.sort {|a,b| a[1]<=>b[1]} .each do |pair|
  print pair[1], " ", pair[0],"\n"
end

tot=node_cnt+way_cnt+ref_cnt
print "finished, #{node_cnt} nodes, #{way_cnt} ways, #{ref_cnt} refs, total:#{tot}\n"


