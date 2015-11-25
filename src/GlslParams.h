#pragma once

#include <map>
#include <vector>
#include <string>

#include "cinder/gl/GlslProg.h"

namespace reza { namespace glsl {

typedef std::shared_ptr<class GlslParams> GlslParamsRef;
class GlslParams
{
public:
    static GlslParamsRef create( const std::string& source ) {
        return GlslParamsRef( new GlslParams( source ) );
    }
    
    GlslParams();
    GlslParams( const std::string& source );
    GlslParams( const GlslParams &copy );
    ~GlslParams();

    void clearUniforms();
    
    void applyUniforms( const ci::gl::GlslProgRef& glslRef );
    void parseUniforms( const std::vector<std::string>& sources );
    void parseUniforms( const std::string& source );
    void parse( const std::string& source );
    
    std::map<std::string, float>& getParams();
    std::map<std::string, std::pair<float, float>>& getRanges();
    
protected:
    std::map<std::string, float> mParams;
    std::map<std::string, std::pair<float, float>> mRanges;
};

} } // namespace reza::glsl