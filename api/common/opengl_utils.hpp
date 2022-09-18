#ifndef OPENGL_UTILS_3_JULY_2016
#define OPENGL_UTILS_3_JULY_2016

#include "GL/glew.h"
#include "gl/GL.h"

#include <string>
#include <vector>

#include <boost/current_function.hpp>

#include "assert_verify.hpp"
#include "requireSemicolon.hpp"

namespace OpenGL
{

inline void checkShaderCompilation( GLuint uiSharder, const char* pszShaderName )
{
    GLint glResult = GL_FALSE;
    glGetShaderiv( uiSharder, GL_COMPILE_STATUS, &glResult );
    if( !glResult )
    {
        std::vector< char > buffer( 1024 );
        GLsizei maxLength = 0U;
        glGetShaderInfoLog( uiSharder, buffer.size(), &maxLength, buffer.data() );
        if( maxLength > 0U )
        {
            std::string strError( buffer.begin(), buffer.begin() + maxLength );
            THROW_RTE( "Error compiling shader: " << pszShaderName << " : " << strError );
        }
        else
        {
            THROW_RTE( "Error compiling shader: " << pszShaderName << " : No shader info log available" );
        }
    }
}

static const char* getErrorString( GLenum error )
{                                                                                                                                                          \
    switch( error )
    {
        case GL_NO_ERROR:
            return "";

        case GL_INVALID_ENUM:
            return "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";

        case GL_INVALID_VALUE:
            return "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";

        case GL_INVALID_OPERATION:
            return "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";

        case GL_OUT_OF_MEMORY:
            return "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";

        case GL_STACK_UNDERFLOW:
            return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";

        case GL_STACK_OVERFLOW:
            return "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
            
        default:
            return "Unknown error";
    }
}

#ifdef _DEBUG

#define CHECK_OPENGL_ERROR( log )                                          \
DO_STUFF_AND_REQUIRE_SEMI_COLON                                     \
(                                                                   \
    ::GLenum openglErrorCode = ::glGetError();                      \
    if( GL_NO_ERROR != openglErrorCode )                            \
    {                                                               \
        log << "FILE " << __FILE__ << ":" << __LINE__ << "\nFUNCTION:" << \
            BOOST_CURRENT_FUNCTION << "\nERROR:" << OpenGL::getErrorString( openglErrorCode ) << "\n";   \
        DEBUG_BREAK( _CRT_ASSERT, OpenGL::getErrorString( openglErrorCode ) );\
    }                                                               \
)

#define CHECK_OPENGL_ERROR_WITH_CODE( log, code )                             \
DO_STUFF_AND_REQUIRE_SEMI_COLON                                     \
(                                                                   \
    ::GLenum openglErrorCode = ::glGetError();                      \
    if( GL_NO_ERROR != openglErrorCode )                            \
    {                                                               \
        log << "FILE " << __FILE__ << ":" << __LINE__ << "\nFUNCTION:" << BOOST_CURRENT_FUNCTION << \
            "\nCODE " << #code << \
            "\nERROR:" << OpenGL::getErrorString( openglErrorCode ) << "\n"; \
        DEBUG_BREAK( _CRT_ASSERT, OpenGL::getErrorString( openglErrorCode ) );\
    }                                                               \
)

#else //_DEBUG

#define CHECK_OPENGL_ERROR( log )                                   \
DO_STUFF_AND_REQUIRE_SEMI_COLON                                     \
(                                                                   \
    ::GLenum openglErrorCode = ::glGetError();                      \
    if( GL_NO_ERROR != openglErrorCode )                            \
    {                                                               \
        log << "FILE " << __FILE__ << ":" << __LINE__ << "\nFUNCTION:" << \
            BOOST_CURRENT_FUNCTION << "\nERROR:" << OpenGL::getErrorString( openglErrorCode ) << "\n";   \
    }                                                               \
)

#define CHECK_OPENGL_ERROR_WITH_CODE( log, code )                             \
DO_STUFF_AND_REQUIRE_SEMI_COLON                                     \
(                                                                   \
    ::GLenum openglErrorCode = ::glGetError();                      \
    if( GL_NO_ERROR != openglErrorCode )                            \
    {                                                               \
        log << "FILE " << __FILE__ << ":" << __LINE__ << "\nFUNCTION:" << BOOST_CURRENT_FUNCTION << \
            "\nCODE " << #code << \
            "\nERROR:" << OpenGL::getErrorString( openglErrorCode ) << "\n"; \
    }                                                               \
)

#endif //_DEBUG

#define CHECK_OPENGL( log, code )    \
DO_STUFF_AND_REQUIRE_SEMI_COLON \
(                               \
    code;                       \
    CHECK_OPENGL_ERROR_WITH_CODE( log, code );  \
)

}

#endif //OPENGL_UTILS_3_JULY_2016
