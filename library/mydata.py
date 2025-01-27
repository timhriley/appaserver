#!/usr/bin/python
#---------------------------
# mydata.py
#
# The class data defines operations to store data objects.
#
# Tim Riley
# 1/99
#-------------------------------------------------------------

import boolean

class mydata:
	def __init__( self ):
		self.data_anything = ''
		self.state_string = 'null'
		self.datatype_string = ''

	def display( self ):
		return `self.get_data()`

	def new_state( self, old_state ):
		if ( old_state == 'null' ): 
			return 'set'
		else: 
			return 'changed'

	# Set section
	# -----------
	def set( self, data ): return self.set_data( data )

	def set_data( self, data ):
		self.data_anything = data
		self.state_string = self.new_state( self.state_string )

	def set_datatype_string( self, s ):
		self.datatype_string = s

	# Get section
	# -----------
	def get( self ): return self.get_data_anything()

	def get_string( self ): return self.get_data()

	def get_data( self ):
		return self.get_data_anything()

	def get_data_anything( self ):
		return self.data_anything

	def get_state_string( self ):
		return self.state_string

	def get_datatype_string( self ):
		return self.datatype_string
# Main
# ----
if ( __name__ == "__main__" ):
	a = mydata()
	a.set_data( 1 )
	print "should be 1 is %s" % a.display()
	print "state should be SET is %s" % a.get_state_string()
	a.set_data( 2 )
	print "should be 2 is %s" % a.display()
	print "state should be changed is %s" % a.get_state_string()
