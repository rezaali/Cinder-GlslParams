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
    clearUniforms();
}

GlslParams::GlslParams( const GlslParams &copy ) {
    
    for( auto& it : copy.mParamOrder ) { mParamOrder[ it.first ] = it.second; }
    
    for( auto& it : copy.mTypeMap ) { mTypeMap[ it.first ] = it.second; }
    for( auto& it : copy.mBoolParams ) { mBoolParams[ it.first ] = it.second; }
 
    for( auto& it : copy.mIntParams ) { mIntParams[ it.first ] = it.second; }
    for( auto& it : copy.mIntRanges ) { mIntRanges[ it.first ] = it.second; }

    for( auto& it : copy.mFloatParams ) { mFloatParams[ it.first ] = it.second; }
    for( auto& it : copy.mFloatRanges ) { mFloatRanges[ it.first ] = it.second; }
    
    for( auto& it : copy.mVec2Params ) { mVec2Params[ it.first ] = it.second; }
    for( auto& it : copy.mVec2Ranges ) { mVec2Ranges[ it.first ] = it.second; }

    for( auto& it : copy.mVec3Params ) { mVec3Params[ it.first ] = it.second; }
    for( auto& it : copy.mVec3Ranges ) { mVec3Ranges[ it.first ] = it.second; }

    for( auto& it : copy.mVec4Params ) { mVec4Params[ it.first ] = it.second; }
    for( auto& it : copy.mVec4Ranges ) { mVec4Ranges[ it.first ] = it.second; }
    
    for( auto& it : copy.mColorParams ) { mColorParams[ it.first ] = it.second; }
}

void GlslParams::clearUniforms() {
    mParamOrder.clear();
    mTypeMap.clear();
    mBoolParams.clear();
    mIntParams.clear();     mIntRanges.clear();
    mFloatParams.clear();   mFloatRanges.clear();
    mVec2Params.clear();    mVec2Ranges.clear();
    mVec3Params.clear();    mVec3Ranges.clear();
    mVec4Params.clear();    mVec4Ranges.clear();
    mColorParams.clear();
}

void GlslParams::applyUniforms( const ci::gl::GlslProgRef& glslRef ) {
    for( auto& it : mBoolParams ) { glslRef->uniform( it.first, it.second ); }
    for( auto& it : mIntParams ) { glslRef->uniform( it.first, it.second ); }
    for( auto& it : mFloatParams ) { glslRef->uniform( it.first, it.second ); }
    for( auto& it : mVec2Params ) { glslRef->uniform( it.first, it.second ); }
    for( auto& it : mVec3Params ) { glslRef->uniform( it.first, it.second ); }
    for( auto& it : mVec4Params ) { glslRef->uniform( it.first, it.second ); }
    for( auto& it : mColorParams ) { glslRef->uniform( it.first, it.second ); }
}

void GlslParams::parseUniforms( const vector<string>& sources ) {
    clearUniforms();
    for( auto& it : sources ) { parse( it ); }
}

void GlslParams::parseUniforms( const string& source ) {
    clearUniforms();
    parse( source );
}

void GlslParams::parse( const string& source ) {
    auto trim = [] ( const string& input, const string& key ) {
        string temp = input;
        size_t foundKey = temp.find( key );
        while( foundKey != string::npos ) {
            temp  = temp.replace( foundKey, key.length(), "" );
            foundKey = temp.find( key );
        }
        return temp;
    };
    
    multimap<string, string> uiTypeMap  = {
        { "int" , "slider" },
        { "int" , "dialer" },
        
        { "float" , "ui" },
        { "float" , "slider" },
        { "float" , "dialer" },
        
        { "vec2" , "pad" },
        { "vec2" , "range" },
        { "vec2" , "ui" },
        
        { "vec3" , "ui" },
        { "vec4" , "ui" },
        { "vec4" , "color" },
        
        { "bool" , "button" },
        { "bool" , "toggle" }
    };
    bool ignore = false;
    vector<string> lines = split( source, '\n' );
    for( auto& it : lines ) {
        string original = it;
        string line = it;
        
        string ignoreStart( "/*" );
        string ignoreEnd( "*/" );
        if( !ignore && line.find( ignoreStart ) != string::npos ) {
            ignore = true;
        }
        
        if( ignore && line.find( ignoreEnd ) == string::npos ) {
            continue;
        } else {
            ignore = false;
        }
        
        std::transform( line.begin(), line.end(), line.begin(), ::tolower );
        string uniform( "uniform " );
        string semicolon( ";" );
        string colon( ":" );
        string space( " " );
        string comment( "//" );
        string comma( "," );
        string newLine( "/n" );
        
        size_t foundUniform = line.find( uniform ); if( foundUniform == string::npos ) { continue; }
        size_t foundComment = line.find( comment ); if( foundComment == string::npos || foundUniform > foundComment ) { continue; }
        size_t foundType = string::npos;
        size_t foundUIType = string::npos;
        string type;
        string uitype;
        string key;
        
        bool valid = false;
        for( auto& ui : uiTypeMap ) {
            foundType = line.find( ui.first );
            string tempkey = comment + ui.second + colon;
            foundUIType = line.find( tempkey );
            if( foundType != string::npos && foundUIType != string::npos ) {
                valid = true;
                type = ui.first;
                uitype = ui.second;
                key = tempkey;
                break; 
            }
        }
        
        if( !valid ) { continue; }
        
        string uniformName = original;
        size_t foundSemicolon = uniformName.find( semicolon );
        uniformName = uniformName.substr( 0, foundSemicolon );
        uniformName = uniformName.replace( foundType, type.length(), "" );
        uniformName = uniformName.replace( foundUniform, uniform.length(), "" );
        uniformName = trim( uniformName, " " );
        
        string uiParams = line.substr( foundUIType + key.length() );
        size_t lastspace = line.rfind( space );
        if( lastspace != string::npos ) {
            uiParams = uiParams.substr( 0, lastspace );
        }
        
        vector<string> params = split( uiParams, "," );
        if( params.size() == 0 ) { continue; }
        
        if( type == "bool" ) {
            mBoolParams[ uniformName ] = stoi( params[ 0 ] ) > 0 ? true : false;
        }
        else if( type == "int" ) {
            mIntParams[ uniformName ] = stoi( params[ 2 ] );
            mIntRanges[ uniformName ] = { stoi( params[ 0 ] ), stoi( params[ 1 ] ) };
        }
        else if( type == "float" ) {
            mFloatParams[ uniformName ] = stof( params[ 2 ] );
            mFloatRanges[ uniformName ] = { stof( params[ 0 ] ), stof( params[ 1 ] ) };
        }
        else if( type == "vec2" ) {
            if( uitype == "range" && params.size() > 3 ) {
                mVec2Params[ uniformName ] = vec2( stof( params[ 2 ] ), stof( params[ 3 ] ) );
            }
            else{
                mVec2Params[ uniformName ] = vec2( stof( params[ 2 ] ) );
            }
            mVec2Ranges[ uniformName ] = { stof( params[ 0 ] ), stof( params[ 1 ] ) };
        }
        else if( type == "vec3" ) {
            mVec3Params[ uniformName ] = vec3( stof( params[ 2 ] ) );
            mVec3Ranges[ uniformName ] = { stof( params[ 0 ] ), stof( params[ 1 ] ) };
        }
        else if( type == "vec4" ) {
            if( uitype == "color" && params.size() > 3 ) {
                ColorA clr;
                clr.set( ColorModel::CM_RGB, vec4( stof( params[ 0 ] ), stof( params[ 1 ] ), stof( params[ 2 ] ), stof( params[ 3 ] ) ) );
                mColorParams[ uniformName ] = clr;
            }
            else {
                mVec4Params[ uniformName ] = vec4( stof( params[ 2 ] ) );
                mVec4Ranges[ uniformName ] = { stof( params[ 0 ] ), stof( params[ 1 ] ) };
            }
        }

        mTypeMap.insert( { uniformName, { type, uitype } } ); 
        mParamOrder[ mParamOrder.size() ] = uniformName;
    }
}

const map<int, string>& GlslParams::getParamOrder() { return mParamOrder; }

const map<string, pair<string, string>>& GlslParams::getTypeMap() { return mTypeMap; }

map<string, bool>& GlslParams::getBoolParams() { return mBoolParams; }

map<string, int>& GlslParams::getIntParams() { return mIntParams; }
map<string, pair<int, int>>& GlslParams::getIntRanges() { return mIntRanges; }

map<string, float>& GlslParams::getFloatParams() { return mFloatParams; }
map<string, pair<float, float>>& GlslParams::getFloatRanges() { return mFloatRanges; }

map<string, vec2>& GlslParams::getVec2Params() { return mVec2Params; }
map<string, pair<float, float>>& GlslParams::getVec2Ranges() { return mVec2Ranges; }

map<string, vec3>& GlslParams::getVec3Params() { return mVec3Params; }
map<string, pair<float, float>>& GlslParams::getVec3Ranges() { return mVec3Ranges; }

map<string, vec4>& GlslParams::getVec4Params() { return mVec4Params; }
map<string, pair<float, float>>& GlslParams::getVec4Ranges() { return mVec4Ranges; }

map<string, ColorA>& GlslParams::getColorParams() { return mColorParams; }