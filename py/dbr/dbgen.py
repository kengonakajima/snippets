# -*- coding: utf-8; -*-


#
# Django settings
#
from django.conf import settings
from django.template import Template, Context
from django.utils.html import escape
from django.utils.safestring import SafeData, EscapeData


import os
os.environ['DJANGO_SETTINGS_MODULE'] = "settings" 


class Database:
    name=None
    version=None
    tables=[]
    def __init__(self,name,version):
        self.name=name
        self.version = version

    def add(self,tbl):
        self.tables.append(tbl)
    def create_sql_source(self, drop=False, create=True ):
        t = Template( open("template.sql" ).read() )
        return t.render( Context( {
                    "db": self,
                    "drop" : drop,
                    "create" : create
                    } ))
    def create_db_proto_xml(self):
        t = Template( open("template.xml" ).read() )
        return t.render( Context( {
                    "db": self,
                    } ) )

    def create_db_sv_cpp(self):
        t = Template( open("svtemplate.cpp" ).read() )
        return t.render( Context( { "db" : self } ) )

    def create_db_sv_h(self):
        t = Template( open("svtemplate.h" ).read() )
        return t.render( Context( { "db" : self } ) )        
        return ""
        
class Field:

    def __init__(self,name,type,primary=False,auto_increment=False, size=None, index=False, choices=None ):
        self.name=name
        self.type=type
        
        self.primary=primary
        self.auto_increment=auto_increment
        self.size=size
        self.index=index
        self.choices=choices

    def type_in_sql(self):
        gen_to_sql={
            "char" : "tinyint",
            "byte" : "tinyint unsigned",
            "short" : "smallint",
            "word" : "smallint unsigned",        
            "int" : "int",
            "dword" : "int unsigned",
            "qword" : "bigint",        
            "string" : "char",
            "blob" : "blob"
            }
        return gen_to_sql[self.type]
    
class Table:
    def __init__(self,name):
        self.name=name
        self.fields=[]
    def add(self, f ):
        self.fields.append(f)


