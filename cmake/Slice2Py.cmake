# original copied from ORCA: http://orca-robotics.sourceforge.net/
# hacked around a bit to reflect a different structure... make that a lot!






# This file contains a set of macros used for generating 
# C++ source from Slice files
#
# Author: Alex Brooks
#

#
# Appends the new_bit to the original.
# If the original is not set, it will be set to the new_bit.
#
MACRO( APPEND original new_bit )

  IF    ( NOT ${original} )
    SET( ${original} ${new_bit} )
  ELSE  ( NOT ${original} )
    SET( ${original} ${${original}} ${new_bit} )
  ENDIF( NOT ${original} )

ENDMACRO( APPEND original new_bit )


# Generate rules for SLICE->C++ files generation, for each
# of the named slice source files.
#
# cast_slice2py( generated_list SRC [INCDIR1 [INCDIR2...]] )
# 
# Returns lists of all the .py files that will be generated.
#   (ie modifies the first argument)
#
# Sets up dependencies: 
#  - Each .py file depends on all the .ice files.
#
# Each optional extra dir is added to the search path for includes

MACRO( cast_slice2py generated_list src_name)

    SET( slice_py_suffixes        _ice.py )
    SET( slice_suffixes            ${slice_py_suffixes} )
    SET( slice2py_command         ${ICE_HOME}/bin/slice2py${EXE_EXTENSION} )
    
    SET( slice_src_dir        	   src/slice)

    get_filename_component( src_path ${src_name} PATH)
    get_filename_component( src_basename ${src_name} NAME)

    IF( src_path )
      SET( slice_src_dir ${src_path})
    ENDIF( src_path )

    FOREACH(slice_inc_dir ${ARGN})
    	APPEND(slice_include_dirs "-I${slice_inc_dir}")
    ENDFOREACH(slice_inc_dir ${ARGN})

    APPEND(slice_include_dirs "-I${PROJECT_SOURCE_DIR}/src/slice")


    # satellite projects need to include slice files from orca installation
    # NOTE: funky interaction between cmake and slice2py: cannot use "" around the slice_args!
    SET( slice_args ${SLICE_PROJECT_ARGS} ${slice_include_dirs} --output-dir ${CMAKE_CURRENT_SOURCE_DIR} )

    APPEND(slice_src_full "${PROJECT_SOURCE_DIR}/${slice_src_dir}/${src_basename}" )
    #MESSAGE( "slice_src_full ${PROJECT_SOURCE_DIR}/${slice_src_dir}/${src_basename}" )


    
        SET( slice_source "${PROJECT_SOURCE_DIR}/${slice_src_dir}/${src_basename}" )
        #	             MESSAGE( "DEBUG: Dealing with ${src_name}")
    

        #
        # Add a custom cmake command to generate each type of output file: headers and source
        #
        FOREACH ( suffix ${slice_suffixes} )
    
            # OUTPUT is the target we're generating rules for.
            STRING( REGEX REPLACE "\\.ice" ${suffix} output_basename "${src_basename}" )
            SET( output_fullname "${CMAKE_CURRENT_SOURCE_DIR}/${output_basename}" )

           #message( "DEBUG output_fullname ${output_fullname}" )

                
            #
            # Add this output to the list of generated files
            #
            IF( ${suffix} STREQUAL "_ice.py" )
                APPEND( ${generated_list} ${output_fullname} )
                SET( generated_file_type "source" )
            ENDIF( ${suffix} STREQUAL "_ice.py" )
        

            #
            # Add the command to generate file.xxx from file.ice
            # Note: when the 'output' is needed, the 'command' will be called with the 'args'
            #
            MESSAGE( STATUS "DEBUG: Adding rule for generating ${output_fullname} from ${src_name}" )
            ADD_CUSTOM_COMMAND(
                OUTPUT  ${output_fullname}
                COMMAND ${slice2py_command} ${slice_args} ${slice_source}
                DEPENDS ${slice_src_full}
		COMMENT "Generating ${generated_file_type} file from ${slice_source}"
                VERBATIM
            )
            set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${output_fullname}")
            STRING( REGEX REPLACE "ice\\.py" "ice.pyc" pyc_fullname "${output_fullname}" )
            set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${pyc_fullname}")
    
        ENDFOREACH ( suffix ${slice_suffixes} )
    

    
   
#     MESSAGE( STATUS "DEBUG: generated_cpp_list: ${${generated_cpp_list}}")
 #      MESSAGE( STATUS "DEBUG: generated_header_list: ${${generated_header_list}}")
    
    MESSAGE( STATUS "Will generate python source files from Slice definition using this command:" )
    MESSAGE( STATUS "${slice2py_command} ${slice_args}" )

ENDMACRO( cast_slice2py generated_list )
