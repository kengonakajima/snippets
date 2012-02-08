import os
import cgi

import wsgiref.handlers

from google.appengine.ext import webapp
from google.appengine.ext.webapp import template
from google.appengine.ext import db
from google.appengine.api import users

class Pet(db.Model):
    name = db.StringProperty(required=True)
    age = db.IntegerProperty()

pet = Pet(name="Fluffy", age=1)
pet.put()

class MainPage(webapp.RequestHandler):
    def get(self):
        path = os.path.join(os.path.dirname(__file__),'hoge.html')
        p = db.GqlQuery("SELECT * FROM Pet")
        for ppp in p:
            print template.render(path,{'n':100, 'p':ppp.name })


def main():
    application = webapp.WSGIApplication([('/', MainPage)],
                                         debug=True)
    wsgiref.handlers.CGIHandler().run(application)

if __name__ == "__main__":
    main()


