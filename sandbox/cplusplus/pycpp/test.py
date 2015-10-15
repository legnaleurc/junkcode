#! /usr/bin/env python
#-*- coding: utf-8 -*-

import pycpp

pc = pycpp.Base.create( 'Derived' )
pc.setName( 'Derived' )
print pc.getName()
