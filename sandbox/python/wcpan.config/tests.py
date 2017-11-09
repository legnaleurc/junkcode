import unittest as ut

import wcpan.config as wc
import wcpan.model as wm


class TestConfiguration(ut.TestCase):

    def test_register(self):
        conf = wc.Configuration('test', AModel)


class AModel(wm.Model):

    version = wm.IntegerField()
