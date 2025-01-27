#!/usr/bin/python
#---------------------------
# boolean.py
#
# The class boolean defines operations to store boolean objects.
#
# Tim Riley
# 1/99
#-------------------------------------------------------------

class boolean:
	def __init__( self, initial_value_int ):
		self.value_int = initial_value_int

	def display( self ):
		return `self.value_int`

	def get( self ):
		return self.value_int

	def set( self ):
		return self.set_true()

	def set_true( self ):
		self.value_int = 1

	def set_false( self ):
		self.value_int = 0

	def set_yn_string( self, yn_string ):
		if ( yn_string == "y" ):
			self.set_true()
		else:
			self.set_false()

# Main
# ----
if ( __name__ == "__main__" ):
	a = boolean( 1 )
	print a.display()
