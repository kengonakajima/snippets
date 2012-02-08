# -*- coding: utf-8; -*-    

from dbgen import Database, Table, Field
import codecs

db = Database(name="test", version=1 )

tbl = Table(name="player")
tbl.add( Field(name="id", type="qword", primary = True, auto_increment = True))
tbl.add( Field( name="loginID", type="string", size=50, index=True))
tbl.add( Field( name="deviceID", type="string", size=50))
tbl.add( Field( name="iconID", type="byte" , choices=[0,1,2,3,4,5,6,7,8,9,10]))
tbl.add( Field( name="level", type="word" ))
tbl.add( Field( name="point", type="dword" ))
tbl.add( Field( name="energy", type="dword" ))
tbl.add( Field( name="longtitude", type="qword" ))
tbl.add( Field( name="latitude", type="qword" ))

db.add(tbl)

tbl = Table(name="playeritem")
tbl.add( Field( name="playerID", type="qword", index=True ))
tbl.add( Field( name="itemTypeID", type="dword"))
tbl.add( Field( name="itemNum", type="dword"))

db.add(tbl)



codecs.open( "itri_table_create.sql","w", "utf-8" ).write(
    db.create_sql_source( drop = True, create = True ) )
codecs.open("dbproto.xml", "w", "utf-8").write(
    db.create_db_proto_xml())
codecs.open("dbsvmain.cpp","w","utf-8").write( db.create_db_sv_cpp())
codecs.open("dbsvmain.h","w","utf-8").write( db.create_db_sv_h())








