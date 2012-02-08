#
#
#
import unittest as ut

class MiniTest( ut.TestCase ):
    def testMiniA(self):
        pass
    def testMiniB(self):
        pass
    
class RingoTest( MiniTest ):
    def testMany(self):
        for i in range(10):
            print "i:",i
            self.testMiniA()
    def testB(self):
        pass


suite = ut.TestLoader().loadTestsFromTestCase( RingoTest )
ut.TextTestRunner( verbosity=2).run(suite)


