#include "GlslParams.h"
#include "cinder/Utilities.h"

using namespace reza::glsl;
using namespace cinder;
using namespace std;

GlslParams::GlslParams() { }

GlslParams::GlslParams( const string& source ) {
    parseUniforms( source );
}

GlslParams::~GlslParams() {
    mParams.clear();
    mRanges.clear();
}

GlslParams::GlslParams( const GlslParams &copy ) {
    for( auto& it : copy.mParams ) { mParams[ it.first ] = it.second; }
    for( auto& it : copy.mRanges ) { mRanges[ it.first ] = it.second; }
}

void GlslParams::clearUniforms() {
    mParams.clear();
    mRanges.clear();
}

void GlslParams::applyUniforms( const ci::gl::GlslProgRef& glslRef ) {
    for( auto& it : mParams ) {
        glslRef->uniform( it.first, it.second );
    }
}

void GlslParams::parseUniforms( const vector<string>& sources ) {
    clearUniforms();
    for( auto& it : sources ) {
        parse( it );
    }
}

void GlslParams::parseUniforms( const string& source ) {
    clearUniforms();
    parse( source );
}

void GlslParams::parse( const string& source ) {
    vector<string> lines = split( source, '\n' );
    for( auto& it : lines ) {
        string line = it;
        
        string uniform ("uniform float ");
        string semicolon (";");
        string space (" ");
        string key ("//UI:");
        string comma (",");
        string newLine ("/n");
        
        size_t foundUniform = line.find( uniform );
        size_t foundUI = line.find( key );
        
        if( foundUniform != string::npos && foundUI != string::npos ) {
            string uiParams = line.substr( foundUI + key.length() );
            
            size_t foundMinimum = uiParams.find( comma );
            string minimum = uiParams.substr( 0, foundMinimum );
            uiParams = uiParams.substr( foundMinimum + 1 );
            
            size_t foundMaximum = uiParams.find( comma );
            string maximum = uiParams.substr( 0, foundMaximum );
            uiParams = uiParams.substr( foundMaximum + 1 );
            
            size_t foundValue = uiParams.find( newLine );
            string value = uiParams.substr( 0, foundValue );
            
            float max = stof( maximum );
            float min = stof( minimum );
            float val = stof( value );
            
            string uniformName = line.substr( uniform.length() );
            size_t foundSemicolon = uniformName.find( semicolon );
            uniformName = uniformName.substr( 0, foundSemicolon );
            size_t foundSpace = uniformName.find( space );
            
            while( foundSpace != string::npos ) {
                uniformName = uniformName.replace( foundSpace, 1, "" );
                foundSpace = uniformName.find( space );
            }
            
            if( mParams.find( uniformName ) == mParams.end() ) {
                mParams[ uniformName ] = val;
                mRanges[ uniformName ] = { min, max };
            }
        }
    }
}

map<string, float>& GlslParams::getParams() {
    return mParams;
}

map<string, pair<float, float>>& GlslParams::getRanges() {
    return mRanges;
}