#!/usr/bin/python
#
# Cair.py
#
# Created on December 22, 2006, 7:22 AM in Java
# Ported to Python June 12, 2007
#
# This code was hastily ported from Java to Python to more easily integrate
# into the GSP build environment. The Java code to begin with was a prototype/
# proof-of-concept so was already largely unstructured. Be kind, it was my first
# python script.
#
# @author efernandez
#
import os
import sys
from xml.dom.ext.reader import Sax2

class Cair: 
    m_coretypes = {}
    m_types = {}
    # 
    # Creates a new instance of Cair 
    #
    def __init__( self, szAirXmlPath ): 
        # Parse the AIR-XML files as XML
        # create Reader object
        reader = Sax2.Reader()
        # parse the document
        f=open(szAirXmlPath)
        self.m_interface = reader.fromStream(f).documentElement
        f.close()
        
        # Initialize the set of Core types
        self.m_coretypes["char"] = "char"
        self.m_coretypes["wchar"] = "wchar_t"
        self.m_coretypes["uchar"] = "unsigned char"
        self.m_coretypes["short"] = "short"
        self.m_coretypes["ushort"] = "unsigned short"
        self.m_coretypes["int32"] = "int"
        self.m_coretypes["uint32"] = "unsigned int"
        self.m_coretypes["int64"] = "long long"
        self.m_coretypes["uint64"] = "unsigned long long"
        self.m_coretypes["float"] = "float"
        self.m_coretypes["double"] = "double"
        self.m_coretypes["string"] = "char*"
        self.m_coretypes["wstring"] = "wchar_t*"
        
        # Initialize the set of all types.. first with the core types
        for key in self.m_coretypes.keys():
            self.m_types[ key ] = self.m_coretypes[ key ]
        
        # Initalize ID to the AIR Core ID
        self.m_szId = "AIR_CORE_ID"
        
        # Initialize the ID for this interface to the hashcode of the service name
        nId = hash(self.m_interface.getAttribute( "name" ))
        
        # Iterate over the defined types
        nl = self.m_interface.getElementsByTagName( "types" )
        for x in range(0, nl.length ):
            elem = nl.item( x )
            
            # Iterate Structs
            structs = elem.getElementsByTagName( "struct" )
            for y in range( 0, structs.length ):
                struct = structs.item( y )
                szType = struct.getAttribute( "type" )
                
                # Add Struct to known type list
                self.m_types[ szType ] = self.getInterfaceName() + "_" + szType
                
                # Add type to the ID of this service                
                nId = ( (nId * 31) + hash( struct.getAttribute( "type" ) ) )
                
                # Iterate over the fields of the struct
                fields = struct.getElementsByTagName( "field" )
                for z in range( 0, fields.length ):
                    field = fields.item( z )
                    # Add each field name and type to the ID of this service
                    nId = ((nId * 31) + hash(field.getAttribute( "name" )))
                    nId = ((nId * 31) + hash(field.getAttribute( "type" )))
            
            # Iterate over imported AIR-XML... Add their defined types..
            #   and also... add their Service ID to this service's ID
            imports = elem.getElementsByTagName( "import" )
            for y in range( 0, imports.length ):
                imp = imports.item( y )
                imported = Cair( imp.getAttribute( "path" ) )
                szImport = imported.getInterfaceName()
                for key in imported.m_types.keys():
                  self.m_types[ key ] = imported.m_types[ key ]
                self.m_szId = "( " + self.m_szId + " * 31 ) + " + imported.m_szId
            
        
        # Iterate over the defined functions
        nl = self.m_interface.getElementsByTagName( "functions" )
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            funcs = elem.getElementsByTagName( "func" )
            for y in range( 0, funcs.length ):
                func = funcs.item( y )
                
                # Add the function to the Service ID calculation
                nId = ((nId * 31) + hash(func.getAttribute( "name" )))
                fields = func.getElementsByTagName( "in" )
                for z in range( 0, fields.length ):
                    # Add each parameter name and type to the Service ID calculation
                    field = fields.item( z )
                    nId = ((nId * 31) + hash(field.getAttribute( "name" )))
                    nId = ((nId * 31) + hash(field.getAttribute( "type" )))
                    
                fields = func.getElementsByTagName( "out" )
                for z in range( 0, fields.length ):
                    # Add each parameter name and type to the Service ID calculation
                    field = fields.item( z );
                    nId = ((nId * 31) + hash(field.getAttribute( "name" )))
                    nId = ((nId * 31) + hash(field.getAttribute( "type" )))
        
        # Calculate final service ID String
        self.m_szId = "( " + self.m_szId + " * 31 ) + " + str( nId % 4294967296 )
    
    
    #
    # Returns the name of the service being defined 
    #
    def getInterfaceName( self ):
        return ( self.m_interface.getAttribute( "name" ) )
    
    
    #
    # Generates the generic Service client code
    #
    def generateService( self, szOutputPath ):
        szIfName = self.getInterfaceName()
        if ( not os.access( szOutputPath, os.F_OK ) ):
            os.makedirs( szOutputPath )
        
        # Generate the header file         
        pos = open( szOutputPath + "/" + szIfName + ".h", "w" )
        pos.write( "/**********************************************************************\n\n" )
        overview = self.m_interface.getElementsByTagName( "overview" )
        pos.write( overview[0].childNodes[0].nodeValue )
        pos.write( "\n **********************************************************************/\n" )
        pos.write( "#ifndef " + szIfName.upper() + "_H\n" );
        pos.write( "#define " + szIfName.upper() + "_H\n" );
        pos.write( "\n" );
        pos.write( "#include <gsp-air.h>\n" );
        pos.write( "\n" );
        pos.write( "#define " + szIfName.upper() + "_SERVICE_ID  ( (long long)(" + self.m_szId  + ") )\n")
        
        # Find any includes from other interfaces for datatypes
        nl = self.m_interface.getElementsByTagName( "types" )
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            imports = elem.getElementsByTagName( "import" )
            for y in range( 0, imports.length ):
                imp = imports.item( y )
                imported = Cair( imp.getAttribute( "path" ) )
                szImport = imported.getInterfaceName()
                pos.write( "#include <" + szImport + ".h>\n" )
                
        pos.write( "\n" )
        
        # Define our datatypes
        nl = self.m_interface.getElementsByTagName( "types" )
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            structs = elem.getElementsByTagName( "struct" )
            for y in range( 0, structs.length ):
                struct = structs.item( y )
                
                szType = struct.getAttribute( "type" )
                
                pos.write( "/**********************************************************************\n" )
                pos.write( " * " + struct.getAttribute( "description" ) + "\n" )
                pos.write( " **********************************************************************/\n" )
                pos.write( "typedef struct _" + szIfName + "_" + szType + "\n" )
                pos.write( "{\n" )
                fields = struct.getElementsByTagName( "field" )
                for z in range( 0, fields.length):
                    field = fields.item( z )
                    szFieldName = field.getAttribute( "name" )
                    szFieldType = field.getAttribute( "type" )
                    bPtr = szFieldType.endswith( "*" )
                    if ( bPtr ): szFieldType = szFieldType[ 0 : len( szFieldType ) - 1 ]
                    bArray = szFieldType.endswith( "]" )
                    if ( bArray & bPtr ):
                        raise IllegalArgumentException, "Arrays cannot be defined as pointers! See Definition of " + szType
                    bFixedArray = False
                    nFixedLen = 0
                    if ( bArray ): 
                        bFixedArray = not szFieldType.endswith( "[]" )
                        n = szFieldType.rfind( '[' )
                        if ( bFixedArray ):
                            nFixedLen = int( szFieldType[ n + 1 : len( szFieldType ) - 1 ] )
                        szFieldType = szFieldType[ 0 : n ]
                    szFieldType = self.m_types[ szFieldType ]
                    if ( bArray & ( not bFixedArray ) ):
                        
                        pos.write( "  int \t" + szFieldName + "_length; /* The length of the " + szFieldName + " field */\n" )
                        pos.write( "  " + szFieldType + "* \t" + szFieldName + "; /* " + field.getAttribute( "description" ) + "*/\n" )
                    else:                        
                        pos.write( "  " + szFieldType )
                        if ( bPtr ): pos.write( "*" )
                        pos.write( " \t" + szFieldName )
                        if ( bArray ): pos.write( "[" + str( nFixedLen ) + "]" )
                        pos.write( "; /* " + field.getAttribute( "description" ) + "*/\n" )
                pos.write( "} " + szIfName + "_" + szType + ";\n" )
                pos.write( "\n" )
                pos.write( "typedef " + szIfName + "_" + szType + "* " + szIfName + "_" + szType + "_t;\n" )
                pos.write( "\n" )
                pos.write( "extern int " + szIfName + "_" + szType + "_serialize( " + szIfName + "_" + szType + "_t, void* );\n" )
                pos.write( "extern int " + szIfName + "_" + szType + "_deserialize( " + szIfName + "_" + szType + "_t, void* );\n" )
                pos.write( "\n" )
        
        pos.write( "extern int " + szIfName + "_rpc( const void**, const char*, void*, void* );\n" )
        pos.write( "\n" )
        
        # Define Function prototypes
        nl = self.m_interface.getElementsByTagName( "functions" )
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            funcs = elem.getElementsByTagName( "func" )
            for y in range( 0, funcs.length ):
                func = funcs.item( y )
                pos.write( "\n" )
                pos.write( "/**********************************************************************\n" )
                pos.write( " * " + func.getAttribute( "description" ) + "\n" )
                params = func.getElementsByTagName( "in" )
                pos.write( " *\n" )
                pos.write( " *  IN: endpoint - The name of the endpoint to execute this function through\n" )
                if ( params.length > 0 ):
                  for z in range( 0, params.length):
                    param = params.item( z )
                    pos.write( " *  IN: " + param.getAttribute( "name" ) + " - " + param.getAttribute( "description" )  + "\n" )
                params = func.getElementsByTagName( "out" )
                if ( params.length > 0 ):
                  pos.write( " *\n" )
                  for z in range( 0, params.length):
                    param = params.item( z )
                    pos.write( " * OUT: " + param.getAttribute( "name" ) + " - " + param.getAttribute( "description" )  + "\n" )
                pos.write( " **********************************************************************/\n" )
                pos.write( "extern int " + szIfName + "_" + func.getAttribute( "name" ) + "( const char* endpoint" )
                params = func.getElementsByTagName( "in" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szType = param.getAttribute( "type" )
                    bPtr = False
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    pos.write( ", " )
                    if ( bPtr or self.m_types.get( szType ).endswith( "*" ) ): pos.write( "const " )
                    pos.write( self.m_types[ szType ] )
                    if ( bPtr ): pos.write( "*" )
                    pos.write( " " + param.getAttribute( "name" ) )

                params = func.getElementsByTagName( "out" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szType = param.getAttribute( "type" )
                    bPtr = False
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    pos.write( ", " + self.m_types[ szType ] )
                    if ( bPtr ): 
                        pos.write( "**" ) 
                    else: 
                        pos.write( "*" )
                    pos.write( " " + param.getAttribute( "name" ) )

                pos.write( " );\n" )
        
        pos.write( "#endif\n" )
        pos.close()

        # Generate the generic client C code
        pos = open( szOutputPath + "/" + szIfName + ".c", "w" )
        pos.write( "#include \"" + szIfName + ".h\"\n" )
        pos.write( "#include <dlog.h>\n" )

        # First generate Serialization/Deserialization routine
        nl = self.m_interface.getElementsByTagName( "types" )
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            structs = elem.getElementsByTagName( "struct" )
            for y in range( 0, structs.length ):
                struct = structs.item( y )
                
                szType = struct.getAttribute( "type" )
                
                pos.write( "\n" )
                pos.write( "int " + szIfName + "_" + szType + "_serialize( " + szIfName + "_" + szType + "_t obj, void* streamptr )\n" )
                pos.write( "{\n" )
                self.declareShared( pos, "  " )
                # Encode each field
                fields = struct.getElementsByTagName( "field" )
                for z in range( 0, fields.length):
                    field = fields.item( z )
                    szFieldName = field.getAttribute( "name" )
                    szFieldType = field.getAttribute( "type" )
                    self.encode( pos, "  ", "obj", "obj->" + szFieldName, szFieldType, "air_write", "streamptr", None )
                pos.write( "\n" )
                pos.write( "  return ( AIR_ERROR_OK );\n" )
                pos.write( "}\n" )
                pos.write( "\n" )
                pos.write( "int " + szIfName + "_" + szType + "_deserialize( " + szIfName + "_" + szType + "_t obj, void* streamptr )\n" )
                pos.write( "{\n" )
                self.declareShared( pos, "  " );
                fields = struct.getElementsByTagName( "field" )
                for z in range( 0, fields.length):
                    field = fields.item( z )
                    szFieldName = field.getAttribute( "name" )
                    szFieldType = field.getAttribute( "type" )
                    self.decode( pos, "  ", "obj", "obj->" + szFieldName, szFieldType, "air_read","streamptr", None )
                pos.write( "\n" )
                pos.write( "  return ( AIR_ERROR_OK );\n" )
                pos.write( "}\n" )
        
        # Generate generic RPC method.. this function is used with all providers..
        #    it is the glue code to unmarshall parameters, call the provider,
        #    and pass results back to the client
        pos.write( "\n" )
        pos.write( "int " + szIfName + "_rpc( const void** funcs, const char* endpoint, void* data, void* streamptr )\n" )
        pos.write( "{\n" )
        self.declareShared( pos, "  " )
        pos.write( "  dlog_printf( DLOG_DEBUG, NULL, \"Endpoint: %s\", endpoint );\n" )
        pos.write( "  long long svcid;\n" )
        pos.write( "  int fn;\n" )
        pos.write( "  ret = air_read( streamptr, &svcid, sizeof( long long ) );\n" )
        pos.write( "  if ( ret ) return ( ret );\n")
        pos.write( "  if ( svcid != " + szIfName.upper() + "_SERVICE_ID ) return ( AIR_ERROR_UNKNOWN_SERVICE );\n")        
        pos.write( "  ret = air_read( streamptr, &fn, sizeof( int ) );\n" )
        pos.write( "  if ( ret ) return ( ret );\n")
        pos.write( "  void* root = air_alloc( NULL, 0 );\n" )
        pos.write( "  switch ( fn )\n" )
        pos.write( "  {\n" )
        
        # Each function has its own unmarshall, execute, marshall sequence
        nl = self.m_interface.getElementsByTagName( "functions" )
        fnum = 1
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            funcs = elem.getElementsByTagName( "func" )
            for y in range( 0, funcs.length ):
                func = funcs.item( y )
                
                pos.write( "    case " + str( fnum ) + ":\n" )
                pos.write( "    {\n" );
                
                szParams = ""
                szSig = ""
                
                # Unmarshall incoming fields
                params = func.getElementsByTagName( "in" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szName = param.getAttribute( "name" )
                    szType = param.getAttribute( "type" )
                    bPtr = False
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    szCType = self.m_types[ szType ]
                    pos.write( "      " + szCType + "* _" + szName + " = (" + szCType + "*)air_alloc( root, " + self.sizeof( szType ) + ");\n" )
                    pos.write( "      if ( _" + szName + " == NULL )\n" )
                    pos.write( "      {\n" )
                    pos.write( "        air_free( root );\n" )
                    pos.write( "        return ( AIR_ERROR_ALLOC_FAILED );\n" )
                    pos.write( "      }\n" )
                    if ( bPtr ):
                        szParams += ", _" + szName
                        szSig += ", const " + szCType + "*"
                    else:
                        szParams += ", *_" + szName
                        szSig += ", "
                        if ( szCType.endswith( "*" ) ):
                            szSig += "const "
                        szSig += szCType
                    self.decode( pos, "      ", "root", "*_" + szName, szType, "air_read", "streamptr", "air_free( root );" )
                    
                # Allocate variables for outbound values
                params = func.getElementsByTagName( "out" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szName = param.getAttribute( "name" )
                    szType = param.getAttribute( "type" )
                    bPtr = False;
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType )- 1 ]
                    szCType = self.m_types.get( szType )
                    pos.write( "      " + szCType + "* _" + szName + " = (" + szCType + "*)air_alloc( root, " + self.sizeof( szType ) + ");\n" )
                    pos.write( "      bzero( _" + szName + ", " + self.sizeof( szType ) + ");" )
                    pos.write( "      if ( _" + szName + " == NULL )\n" )
                    pos.write( "      {\n" )
                    pos.write( "        air_free( root );\n" )
                    pos.write( "        return ( AIR_ERROR_ALLOC_FAILED );\n" )
                    pos.write( "      }\n" )
                    if ( bPtr ):
                        szParams += ", &_" + szName
                        szSig += ", " + szCType
                    else:
                        szParams += ", _" + szName
                        szSig += ", " + szCType + "*"
                
                # Call Function
                pos.write( "      dlog_printf( DLOG_DEBUG, NULL, \"Calling RPC Function #" + str(fnum) + "\" );\n" )
                pos.write( "      ret = ((int(*)(const char*, void*" + szSig + "))funcs[ " + str(fnum) + " ])( endpoint, data" + szParams + " );\n")
                pos.write( "      if ( ret )\n" )
                pos.write( "      {\n" )
                pos.write( "        air_free( root );\n" )
                pos.write( "        return ( ret );\n" )
                pos.write( "      }\n" )
                
                # Marshall return value
                pos.write( "      ret = AIR_ERROR_OK;\n" )
                pos.write( "      ret = air_write( streamptr, &ret, sizeof( int ) );\n" )
                pos.write( "      if ( ret )\n" )
                pos.write( "      {\n" )
                pos.write( "        air_free( root );\n" )
                pos.write( "        return ( ret );\n" )
                pos.write( "      }\n" )
                
                # Marshall out parameters
                params = func.getElementsByTagName( "out" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szName = param.getAttribute( "name" )
                    szType = param.getAttribute( "type" )
                    bPtr = False;
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    szCType = self.m_types.get( szType )
                    
                    self.encode( pos, "      ", "root", "*_" + szName, szType, "air_write", "streamptr", "air_free( root );" )
                
                pos.write( "      air_free( root );\n" )
                pos.write( "      return ( AIR_ERROR_OK );\n" )
                pos.write( "    }\n" )
                pos.write( "\n" )                
                fnum = fnum + 1
                
        pos.write( "    default:\n" )
        pos.write( "      air_free( root );\n" )
        pos.write( "      return ( AIR_ERROR_UNKNOWN_FUNCTION );\n" )
        pos.write( "  }\n" )
        pos.write( "}\n" )
        pos.write( "\n" )
        
        # Now generate each client method... this method handles
        #   local providers and remote marhsalling and unmarshalling
        fnum = 1
        nl = self.m_interface.getElementsByTagName( "functions" )
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            funcs = elem.getElementsByTagName( "func" )
            for y in range( 0, funcs.length ):
                # Generate function formal parameters
                szParams = ""
                szSig = ""
                func = funcs.item( y )
                pos.write( "int " + szIfName + "_" + func.getAttribute( "name" ) + "( const char* endpoint" )
                params = func.getElementsByTagName( "in" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szType = param.getAttribute( "type" )
                    bPtr = False;
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    pos.write( ", " )
                    if ( bPtr or self.m_types.get( szType ).endswith( "*" ) ): pos.write( "const " )
                    pos.write( self.m_types.get( szType ) )
                    if ( bPtr ): pos.write( "*" )
                    pos.write( " " + param.getAttribute( "name" ) )
                    szParams += ", " + param.getAttribute( "name" )
                    szSig += ", "
                    if ( bPtr or self.m_types.get( szType ).endswith( "*" ) ): szSig += "const "
                    szSig += self.m_types.get( szType )
                    if ( bPtr ): szSig += "*"
                
                params = func.getElementsByTagName( "out" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szType = param.getAttribute( "type" )
                    bPtr = False
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    pos.write( ", " + self.m_types.get( szType ) )
                    if ( bPtr ): pos.write( "*" )
                    pos.write( "* " + param.getAttribute( "name" ) )
                    szParams += ", " + param.getAttribute( "name" )
                    szSig += ", " + self.m_types.get( szType ) + "*"
                    if ( bPtr ): szSig += "*"
                
                pos.write( " )\n" )               
                pos.write( "{\n" )
                
                # Declare common variables
                self.declareShared( pos, "  " )
                
                # Open the service
                pos.write( "  air_service svc;\n" )
                pos.write( "  if ( air_open_service( &svc, endpoint ) )\n" )
                pos.write( "  {\n" )
                pos.write( "    return ( AIR_ERROR_UNKNOWN_SERVICE );\n" )
                pos.write( "  }\n" )
                
                # If the service is local.. call the provider function directly
                pos.write( "  else if ( svc.local )\n" )
                pos.write( "  {\n" )
                pos.write( "    ret = (((int(*)(const char*, void*" + szSig + "))svc.funcs[ " + str( fnum ) + " ])( endpoint, svc.data" + szParams + " ) );\n" )
                pos.write( "    air_close_service( &svc, 0 );\n" )
                pos.write( "    return ( ret );\n" )
                pos.write( "  }\n" )
                
                # Otherwise this is remote
                pos.write( "  else\n" ) 
                pos.write( "  {\n" )                
                pos.write( "    long long svcid = " + szIfName.upper() + "_SERVICE_ID;\n" )
                pos.write( "    for ( ;; )\n" )
                pos.write( "    {\n" )      
                
                # Connect to the remote service... this may be repeated
                #   in the event of a reused connection failing
                pos.write( "      if ( air_connect_service( &svc ) )\n")
                pos.write( "      {\n")
                pos.write( "        air_close_service( &svc, 0 );\n")
                pos.write( "        return ( AIR_ERROR_UNKNOWN_SERVICE );\n")
                pos.write( "      }\n")
                
                # Send Service ID
                pos.write( "      ret = air_write( svc.streamptr, &svcid, sizeof( long long ) );\n" )
                pos.write( "      if ( ret )\n" )
                pos.write( "      {\n" )
                pos.write( "        if ( ( ret == AIR_ERROR_WRITE_ERROR ) || ( ret == AIR_ERROR_WRITE_TIMEOUT ) ) continue;\n" )
                pos.write( "        air_close_service( &svc, 0 );\n" )
                pos.write( "        return ( ret );\n" )
                pos.write( "      }\n" )
                
                # Send Function ID
                pos.write( "      int fnid = " + str( fnum ) + ";\n" )
                pos.write( "      ret = air_write( svc.streamptr, &fnid, sizeof( int ) );\n" )
                pos.write( "      if ( ret )\n" )
                pos.write( "      {\n" )
                pos.write( "        if ( ( ret == AIR_ERROR_WRITE_ERROR ) || ( ret == AIR_ERROR_WRITE_TIMEOUT ) ) continue;\n" )
                pos.write( "        air_close_service( &svc, 0 );\n" )
                pos.write( "        return ( ret );\n" )
                pos.write( "      }\n" )
                
                # Marshall Input Parameters
                params = func.getElementsByTagName( "in" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szName = param.getAttribute( "name" )
                    szType = param.getAttribute( "type" )
                    self.encode( pos, "      ", szName, szName, szType, "air_write", "svc.streamptr", "if ( ( ret == AIR_ERROR_WRITE_ERROR ) || ( ret == AIR_ERROR_WRITE_TIMEOUT ) ) continue;\nair_close_service( &svc, 0 );" )
                
                # Read Result
                pos.write( "      int tret;\n" )
                pos.write( "      ret = air_read( svc.streamptr, &tret, sizeof( int ) );\n" )
                pos.write( "      if ( ret )\n" )
                pos.write( "      {\n" )
                pos.write( "        if ( ( ret == AIR_ERROR_WRITE_ERROR ) || ( ret == AIR_ERROR_WRITE_TIMEOUT ) ) continue;\n" )
                pos.write( "        air_close_service( &svc, 0 );\n" )
                pos.write( "        return ( ret );\n" )
                pos.write( "      }\n" )
                # If Ret == 0 Read Output
                pos.write( "      if ( tret == AIR_ERROR_OK )\n" )
                pos.write( "      {\n" );
                
                # Unmarshall Output
                params = func.getElementsByTagName( "out" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szName = param.getAttribute( "name" )
                    szType = param.getAttribute( "type" )
                    bPtr = False
                    if ( szType.endswith( "*" ) ):
                        self.decode( pos, "        ", "NULL", szName, szType, "air_read", "svc.streamptr", "air_close_service( &svc, 0 );" )
                    else:
                        self.decode( pos, "        ", szName, "*" + szName, szType, "air_read", "svc.streamptr", "air_close_service( &svc, 0 );" )
                pos.write( "      }\n" )                    
                pos.write( "      air_close_service( &svc, 1 );\n" )
                pos.write( "      return ( tret );\n" )
                pos.write( "    }\n" )
                pos.write( "  }\n" )                    
                pos.write( "}\n" )
                pos.write( "\n" )
                
                fnum = fnum + 1
        
        pos.close()
        
    #
    #This method generates the header and initial C code for a provider
    #
    def generateProvider( self, szOutputPath, szPrefix ):
        szIfName = self.getInterfaceName();
        
        if ( not os.access( szOutputPath, os.F_OK ) ):
            os.makedirs( szOutputPath )
        
        functions = []
        
        # Since the C file is user edited.. only generate it if we won't overwrite an existing file!
        if ( os.access( szOutputPath + "/" + szPrefix + "_" + szIfName + ".c", os.F_OK ) ):
          raise NameError, "ERROR! Could not generate Provider because a provider with the same name already exists!"
        
        # Generate C file!
        pos = open( szOutputPath + "/" + szPrefix + "_" + szIfName + ".c", "w" );
        pos.write( "#include \"" + szPrefix + "_" + szIfName + ".h\"\n" )
        pos.write( "\n" )
        
        functions.append( szIfName + "_rpc" )
        
        # Output a formal parameter list for each function and have
        #  a function body that simply returns the OK status
        nl = self.m_interface.getElementsByTagName( "functions" )
        for x in range( 0, nl.length ):
            elem = nl.item( x )
            funcs = elem.getElementsByTagName( "func" )
            for y in range( 0, funcs.length ):
                func = funcs.item( y )
                pos.write( "static int " + szPrefix + "_" + szIfName + "_" + func.getAttribute( "name" ) + "( const char* endpoint, void* data" )
                functions.append( szPrefix + "_" + szIfName + "_" + func.getAttribute( "name" ) )
                params = func.getElementsByTagName( "in" );
                for z in range( 0, params.length):
                    param = params.item( z )
                    szType = param.getAttribute( "type" )
                    bPtr = False
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    pos.write( ", " )
                    if ( bPtr or self.m_types.get( szType ).endswith( "*" ) ): pos.write( "const " )
                    pos.write( self.m_types[ szType ] )
                    if ( bPtr ): pos.write( "*" )
                    pos.write( " " + param.getAttribute( "name" ) )
                
                params = func.getElementsByTagName( "out" )
                for z in range( 0, params.length):
                    param = params.item( z )
                    szType = param.getAttribute( "type" )
                    bPtr = False
                    if ( szType.endswith( "*" ) ):
                        bPtr = True
                        szType = szType[ 0 : len( szType ) - 1 ]
                    pos.write( ", " + self.m_types[ szType ] )
                    if ( bPtr ): pos.write( "*" )
                    pos.write( "* " + param.getAttribute( "name" ) )

                pos.write( " )\n" )
                pos.write( "{\n" )
                pos.write( "\n" )
                pos.write( "  /* Implementation Should Go Here! */ \n" )
                pos.write( "\n" )
                pos.write( "  return ( AIR_ERROR_OK );\n" )
                pos.write( "}\n" )
                pos.write( "\n" )
        
        # Declare the function pointer array for this provider
        pos.write( "const void* " + szPrefix + "_" + szIfName + "_provider[] = {\n" )
        for fn in functions:
            pos.write( "  " + fn + ",\n" )
        pos.write( "};\n" );
        pos.close();
        
        # Output the header file for the provider, which is merely a extern reference to the provider array
        pos = open( szOutputPath + "/" + szPrefix + "_" + szIfName + ".h", "w" );
        pos.write( "#ifndef " + szIfName.upper() + "_" + szPrefix.upper() + "_H\n" )
        pos.write( "#define " + szIfName.upper() + "_" + szPrefix.upper() + "_H\n" )
        pos.write( "\n" )
        pos.write( "#include <" + szIfName + ".h>\n" )
        pos.write( "\n" )
        pos.write( "extern const void* " + szPrefix + "_" + szIfName + "_provider[" + str ( len( functions ) ) + "];\n" )
        pos.write( "\n" )
        pos.write( "#endif\n" )
        pos.close()
    
    
    #
    # The following methods jsut output C code that is reused in multiple places
    #
    
    #
    # Declare common variables
    #
    def declareShared( self, pos, prefix ):
        pos.write( prefix + "int iter, ret, len;\n" )
        pos.write( prefix + "char tmp;\n" )
    
    #
    # Marshalls a variable
    #
    def encode( self, pos, prefix, parentPtr, name, ctype, writeFn, writePtr, cleanupCode ):
        # pos.write( prefix + "dlog_printf( DLOG_DEBUG, NULL, \"Encoding " + name + " as " + ctype + "\" );\n" )
        if ( ctype.endswith( "*" ) ): # Check for pointer... pointers are either NULL or encoded as their child type
            pos.write( prefix + "if ( " + name + " == NULL )\n" )
            pos.write( prefix + "{\n" )
            pos.write( prefix + "  tmp = 0;\n" )
            pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", &tmp, sizeof( char ) );\n" )
            self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
            pos.write( prefix + "}\n" )
            pos.write( prefix + "else\n" )
            pos.write( prefix + "{\n" )
            pos.write( prefix + "  tmp = 1;\n" )
            pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", &tmp, sizeof( char ) );\n" )
            self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
            self.encode( pos, prefix + "  ", parentPtr, "*(" + name + ")", ctype[ 0 : len( ctype ) - 1 ], writeFn, writePtr, cleanupCode )
            pos.write( prefix + "}\n" )
        elif ( ctype.endswith( "]" ) ): # Array!
            if ( ctype.endswith( "[]" ) ):
                pos.write( prefix + "len = " + name + "_length;\n" )
                pos.write( prefix + "ret = " + writeFn + "( " + writePtr + ", &len, sizeof( int ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "for ( iter = 0; iter < len; iter++ )\n" )
                pos.write( prefix + "{\n" )
                self.encode( pos, prefix + "  ", parentPtr, name + "[ iter ]", ctype[ 0 : len( ctype ) - 2 ], writeFn, writePtr, cleanupCode )
                pos.write( prefix + "}\n" )
            else:
                z = ctype.rfind( '[' )
                pos.write( prefix + "len = " + ctype[ z + 1 : len( ctype ) - 1 ] + ";\n" )
                pos.write( prefix + "for ( iter = 0; iter < len; iter++ )\n" )
                pos.write( prefix + "{\n" )
                self.encode( pos, prefix + "  ", parentPtr, name + "[ iter ]", ctype[ 0 : z ], writeFn, writePtr, cleanupCode )
                pos.write( prefix + "}\n" )
        else:
            if ( ctype== ( "string" ) ):
                pos.write( prefix + "if ( " + name + " == NULL )\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + "  len = -1;\n" )
                pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", &len, sizeof( int ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "}\n" )
                pos.write( prefix + "else\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + "  len = strlen( " + name + " );\n" )
                pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", &len, sizeof( int ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", " + name + ", len * sizeof( char ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "}\n" )
            elif ( ctype== ( "wstring" ) ):
                pos.write( prefix + "if ( " + name + " == NULL )\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + "  len = -1;\n" )
                pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", &len, sizeof( int ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "}\n" )
                pos.write( prefix + "else\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + "  len = wcslen( " + name + " );\n" )
                pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", &len, sizeof( int ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "  ret = " + writeFn + "( " + writePtr + ", " + name + ", len * sizeof( wchar_t ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "}\n" )
            elif ( self.m_coretypes.has_key( ctype ) ):
                pos.write( prefix + "ret = " + writeFn + "( " + writePtr + ", &(" + name + "), sizeof( " + self.m_coretypes[ ctype ] + " ) );\n" )
                self.cleanup( pos, prefix, "ret != AIR_ERROR_OK", "ret", cleanupCode )
            else:
                ctype = self.m_types[ ctype ]
                pos.write( prefix + "ret = " + ctype + "_serialize( &(" + name + "), " + writePtr + " );\n" )
                self.cleanup( pos, prefix, "ret != AIR_ERROR_OK", "ret", cleanupCode )
    
    #
    # Unmarshalls a variable
    #
    def decode( self, pos, prefix, parentPtr, name, ctype, readFn, readPtr, cleanupCode ):
        # pos.write( prefix + "dlog_printf( DLOG_DEBUG, NULL, \"Decoding " + name + " as " + ctype + "\" );\n" )
        if ( ctype.endswith( "*" ) ): # Check for pointer... pointers are either NULL or encoded as their child type
            pos.write( prefix + "ret = " + readFn + "( " + readPtr + ", &tmp, sizeof( char ) );\n" )
            self.cleanup( pos, prefix, "ret != AIR_ERROR_OK", "ret", cleanupCode )
            pos.write( prefix + "if ( tmp == 0 )\n" )
            pos.write( prefix + "{\n" )
            pos.write( prefix + "  " + name + " = NULL;\n" )
            pos.write( prefix + "}\n" )
            pos.write( prefix + "else\n" )
            pos.write( prefix + "{\n" )
            pos.write( prefix + "  " + name + " = air_alloc( " + parentPtr + ", " + self.sizeof( ctype[ 0 : len( ctype ) - 1 ] ) + ");\n" )
            self.cleanup( pos, prefix + "  ", name + " == NULL", "AIR_ERROR_ALLOC_FAILED", cleanupCode )
            self.decode( pos, prefix + "  ", parentPtr, "*(" + name +")", ctype[ 0 : len( ctype ) - 1 ], readFn, readPtr, cleanupCode )
            pos.write( prefix + "}\n" )
        elif ( ctype.endswith( "]" ) ): # Array!
            if ( ctype.endswith( "[]" ) ):
                pos.write( prefix + "ret = " + readFn + "( " + readPtr + ", &len, sizeof( int ) );\n" )
                self.cleanup( pos, prefix, "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + name + "_length = len;\n" )
                pos.write( prefix + name + " = air_alloc( " + parentPtr + ", len * " + self.sizeof( ctype[ 0 : len( ctype ) - 2 ] ) + ");\n" )
                self.cleanup( pos, prefix, name + " == NULL", "AIR_ERROR_ALLOC_FAILED", cleanupCode )
                pos.write( prefix + "for ( iter = 0; iter < len; iter++ )\n" )
                pos.write( prefix + "{\n" )
                self.decode( pos, prefix + "  ", parentPtr, name + "[ iter ]", ctype[ 0 : len( ctype ) - 2 ], readFn, readPtr, cleanupCode )
                pos.write( prefix + "}\n" )
            else:
                z = ctype.rfind( '[' )                
                pos.write( prefix + "len = " + ctype[ z + 1 : len( ctype ) - 1 ] + ";\n" )
                pos.write( prefix + "for ( iter = 0; iter < len; iter++ )\n" )
                pos.write( prefix + "{\n" )
                self.decode( pos, prefix + "  ", parentPtr, name + "[ iter ]", ctype[ 0 : z ], readFn, readPtr, cleanupCode )
                pos.write( prefix + "}\n" )
        else:
            if ( ctype== ( "string" ) ):
                pos.write( prefix + "ret = " + readFn + "( " + readPtr + ", &len, sizeof( int ) );\n" )
                self.cleanup( pos, prefix, "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "if ( len == -1 )\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + name + " = NULL;\n" )
                pos.write( prefix + "}\n" )
                pos.write( prefix + "else\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + "  " + name + " = air_alloc( " + parentPtr + ", ( len + 1 ) * sizeof( char ) );\n" )
                self.cleanup( pos, prefix + "  ", name + " == NULL", "AIR_ERROR_ALLOC_FAILED", cleanupCode )
                pos.write( prefix + "  (" + name + ")[ len ] = 0;\n" )
                pos.write( prefix + "  ret = " + readFn + "( " + readPtr + ", " + name + ", len * sizeof( char ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "}\n" )
            elif ( ctype== ( "wstring" ) ):
                pos.write( prefix + "ret = " + readFn + "( " + readPtr + ", &len, sizeof( int ) );\n" )
                pos.write( prefix + "if ( len == -1 )\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + name + " = NULL;\n" )
                pos.write( prefix + "}\n" )
                pos.write( prefix + "else\n" )
                pos.write( prefix + "{\n" )
                pos.write( prefix + "  " + name + " = air_alloc( " + parentPtr + ", ( len + 1 ) * sizeof( wchar_t ) );\n" )
                self.cleanup( pos, prefix + "  ", name + " == NULL", "AIR_ERROR_ALLOC_FAILED", cleanupCode )
                pos.write( prefix + "  (" + name +")[ len ] = 0;\n" )
                pos.write( prefix + "  ret = " + readFn + "( " + readPtr + ", " + name + ", len * sizeof( wchar_t ) );\n" )
                self.cleanup( pos, prefix + "  ", "ret != AIR_ERROR_OK", "ret", cleanupCode )
                pos.write( prefix + "}\n" )
            elif ( self.m_coretypes.has_key( ctype ) ):
                pos.write( prefix + "ret = " + readFn + "( " + readPtr + ", &(" + name + "), sizeof( " + self.m_coretypes[ ctype ] + " ) );\n" )
                self.cleanup( pos, prefix, "ret != AIR_ERROR_OK", "ret", cleanupCode )
            else:
                ctype = self.m_types[ ctype ]
                pos.write( prefix + "ret = " + ctype + "_deserialize( &(" + name + "), " + readPtr + " );\n" )
                self.cleanup( pos, prefix, "ret != AIR_ERROR_OK", "ret", cleanupCode )
    
    #
    # Returns a size expression suitable for use with memory allocation for a given AIR-XML type
    #
    def sizeof( self, ctype ):
        if ( ( ctype.endswith( "*" ) ) | ( ctype.endswith( "[]" ) ) | ( ctype.endswith( "string" ) ) | ( ctype.endswith( "wstring" ) ) ):
            return ( "sizeof( void* )" );
        elif ( ctype.endswith( "]" ) ):
            return int( ctype[ ctype.rfind( '[' ) + 1 : len( ctype ) - 1 ] ) + " * " + sizeof( ctype[ 0 : ctype.rfind( '[' ) ] )
        else:
            return ( "sizeof(" + self.m_types.get( ctype ) + ")" )
    
    #
    # Output code used to cleanup state in the event of a failure
    #
    def cleanup( self, pos, prefix, test, ret, cleanup ):
        pos.write( prefix + "if ( " + test + " )\n")
        pos.write( prefix + "{\n");
        if ( cleanup != None ):
            lines = cleanup.split( '\n' )
            for line in lines:
                pos.write( prefix + "  " + line + "\n" )
        pos.write( prefix + "  return ( " + ret + " );\n" )
        pos.write( prefix + "}\n")
    


#
# Output the Usage rules for the CAIR tool
#
def usage():
    print( "Compile AIR Tool Usage" );
    print( "" )
    print( "Service Generation" );
    print( "Usage: " + sys.argv[0] + " -service [AIR-XML File Path] [Output Path]");
    print( "Example: " + sys.argv[0] + " -service ./calculator.airxml .");
    print( "" )
    print( "Provider Generation" );
    print( "Usage: " + sys.argv[0] + " -provider [AIR-XML File Path] [Output Path] [prefix]");
    print( "Example: " + sys.argv[0] + " -provider ./calculator.airxml . mycalc");

# Main method for CAIR
if ( len(sys.argv) == 4 ):
    if ( sys.argv[1] == "-service" ):
        Cair( sys.argv[2] ).generateService( sys.argv[3] )
    else:
        usage()
elif ( len(sys.argv) == 5 ):
    if ( sys.argv[1] == "-provider" ):
        Cair( sys.argv[2] ).generateProvider( sys.argv[3], sys.argv[4] )
    else:
        usage()
else:
    usage()

