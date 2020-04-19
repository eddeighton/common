cmake_minimum_required(VERSION 2.8)

function( link_common targetname )
	target_link_libraries( ${targetname} commonlib )
endfunction( link_common )
