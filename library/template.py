#!/usr/bin/python
#---------------------------
# sample.py
#
# This are the operations for "sample" in the collecting samples
# activity.
#
# Tim Riley
# 1/99
#-----------------------------------------------------------

import sys, os

class sample:
	not_null = [	'sample_id', 		\
			'method_id', 		\
			'sample_number',	\
			'observing_id' ]

	# Initialize section
	# ------------------
	def __init__( self ):
		self.dictionary = {}
		self.dictionary[ 'sample_id' ] = 'null'
		self.dictionary[ 'method_id' ] = 'null'
		self.dictionary[ 'sample_number' ] = 'null'
		self.dictionary[ 'observing_id' ] = 'null'
		self.dictionary[ 'water_depth' ] = 'null'
		self.dictionary[ 'sediment_depth' ] = 'null'
		self.dictionary[ 'compaction' ] = 'null'
		self.dictionary[ 'vial_number' ] = 'null'

	# Set section
	# -----------
	def set_sample_id( self, a ):
        	self.dictionary[ 'sample_id' ] = a

	def set_method_id( self, a ):
        	self.dictionary[ 'method_id' ] = a

	def set_sample_number( self, a ):
        	self.dictionary[ 'sample_number' ] = a

	def set_observing_id( self, a ):
        	self.dictionary[ 'observing_id' ] = a

	def set_water_depth( self, a ):
        	self.dictionary[ 'water_depth' ] = a

	def set_sediment_depth( self, a ):
        	self.dictionary[ 'sediment_depth' ] = a

	def set_compaction( self, a ):
        	self.dictionary[ 'compaction' ] = a

	def set_vial_number( self, a ):
        	self.dictionary[ 'vial_number' ] = a

	# Get section
	# -----------
	def get_sample_id( self ):
        	return self.dictionary[ 'sample_id' ]

	def get_method_id( self ):
        	return self.dictionary[ 'method_id' ]

	def get_sample_number( self ):
        	return self.dictionary[ 'sample_number' ]

	def get_observing_id( self ):
        	return self.dictionary[ 'observing_id' ]

	def get_water_depth( self ):
        	return self.dictionary[ 'water_depth' ]

	def get_sediment_depth( self ):
        	return self.dictionary[ 'sediment_depth' ]

	def get_compaction( self ):
        	return self.dictionary[ 'compaction' ]

	def get_vial_number( self ):
        	return self.dictionary[ 'vial_number' ]

	# Process section
	# ---------------
	def all_nulls_present( self ):
		for a in self.not_null:
			if ( self.dictionary[ a ] == 'null' ):
				print "Field %s is null" % a
				return 0
		return 1

	def blank_out_nulls( self ):
		for a in self.dictionary.keys():
			if ( self.dictionary[ a ] == 'null' ):
				self.dictionary[ a ] = ''

	def get_sys_str( self, debug ):
		sys_str = "echo \'%s|%s|%s|%s|%s|%s|%s|%s\' 	|\
			   insert_oracle.sh riley sample %s"	%\
			  ( self.get_sample_id(),
			    self.get_method_id(),
			    self.get_sample_number(),
			    self.get_observing_id(),
			    self.get_water_depth(),
			    self.get_sediment_depth(),
			    self.get_compaction(),
			    self.get_vial_number(),
			    debug )

		return sys_str

	def insert_sample( self, debug ):
		if ( self.all_nulls_present() == 0 ):
			return 0

		self.blank_out_nulls()
		sys_str = self.get_sys_str( debug )

		if ( debug == 'debug' ):
			print "(%s)" % sys_str

		os.system( sys_str )
		return 1

# Test drive
# ----------
if ( __name__ == "__main__" ):

	s = sample()

	s.set_sample_id( "55" )
	s.set_method_id( "2" )
	s.set_sample_number( "1" )
	s.set_observing_id( "55" )

	print "Inserting"
	print "---------"
	print "%s" % s.insert_sample( "debug" )

	sys.exit( 0 )
	
