#include "GlslParams.h"
#include "cinder/Utilities.h"

using namespace reza::glsl;
using namespace cinder;
using namespace std;

GlslParams::GlslParams()
{
}

GlslParams::GlslParams( const string &source )
{
	parseUniforms( source );
}

GlslParams::~GlslParams()
{
	clearUniforms();
}

GlslParams::GlslParams( const GlslParams &copy )
{
	for( auto &it : copy.mParamOrder ) {
		mParamOrder[it.first] = it.second;
	}

	for( auto &it : copy.mTypeMap ) {
		mTypeMap[it.first] = it.second;
	}
	for( auto &it : copy.mBoolParams ) {
		mBoolParams[it.first] = it.second;
	}

	for( auto &it : copy.mIntParams ) {
		mIntParams[it.first] = it.second;
	}
	for( auto &it : copy.mIntRanges ) {
		mIntRanges[it.first] = it.second;
	}

	for( auto &it : copy.mFloatParams ) {
		mFloatParams[it.first] = it.second;
	}
	for( auto &it : copy.mFloatRanges ) {
		mFloatRanges[it.first] = it.second;
	}

	for( auto &it : copy.mVec2Params ) {
		mVec2Params[it.first] = it.second;
	}
	for( auto &it : copy.mVec2Ranges ) {
		mVec2Ranges[it.first] = it.second;
	}

	for( auto &it : copy.mVec3Params ) {
		mVec3Params[it.first] = it.second;
	}
	for( auto &it : copy.mVec3Ranges ) {
		mVec3Ranges[it.first] = it.second;
	}

	for( auto &it : copy.mVec4Params ) {
		mVec4Params[it.first] = it.second;
	}
	for( auto &it : copy.mVec4Ranges ) {
		mVec4Ranges[it.first] = it.second;
	}

	for( auto &it : copy.mColorParams ) {
		mColorParams[it.first] = it.second;
	}
}

void GlslParams::clearUniforms()
{
	mParamOrder.clear();
	mTypeMap.clear();
	mBoolParams.clear();
	mIntParams.clear();
	mIntRanges.clear();
	mFloatParams.clear();
	mFloatRanges.clear();
	mVec2Params.clear();
	mVec2Ranges.clear();
	mVec3Params.clear();
	mVec3Ranges.clear();
	mVec4Params.clear();
	mVec4Ranges.clear();
	mColorParams.clear();
}

void GlslParams::applyUniforms( const ci::gl::GlslProgRef &glslRef )
{
	for( auto &it : mBoolParams ) {
		glslRef->uniform( it.first, it.second );
	}
	for( auto &it : mIntParams ) {
		glslRef->uniform( it.first, it.second );
	}
	for( auto &it : mFloatParams ) {
		glslRef->uniform( it.first, it.second );
	}
	for( auto &it : mVec2Params ) {
		glslRef->uniform( it.first, it.second );
	}
	for( auto &it : mVec3Params ) {
		glslRef->uniform( it.first, it.second );
	}
	for( auto &it : mVec4Params ) {
		glslRef->uniform( it.first, it.second );
	}
	for( auto &it : mColorParams ) {
		glslRef->uniform( it.first, it.second );
	}
}

void GlslParams::parseUniforms( const vector<string> &sources )
{
	clearUniforms();
	for( auto &it : sources ) {
		parse( it );
	}
}

void GlslParams::parseUniforms( const string &source )
{
	clearUniforms();
	parse( source );
}

void GlslParams::parse( const string &source )
{
	auto trim = []( const string &input, const string &key ) {
		string temp = input;
		size_t foundKey = temp.find( key );
		while( foundKey != string::npos ) {
			temp = temp.replace( foundKey, key.length(), "" );
			foundKey = temp.find( key );
		}
		return temp;
	};

	multimap<string, string> uiTypeMap = {
		{ "int", "slider" },
		{ "int", "dialer" },

		{ "float", "ui" },
		{ "float", "slider" },
		{ "float", "dialer" },

		{ "vec2", "pad" },
		{ "vec2", "range" },
		{ "vec2", "ui" },
		{ "vec2", "slider" },
		{ "vec2", "dialer" },

		{ "vec3", "ui" },
		{ "vec3", "slider" },
		{ "vec3", "dialer" },

		{ "vec4", "ui" },
		{ "vec4", "slider" },
		{ "vec4", "color" },
		{ "vec4", "dialer" },

		{ "bool", "button" },
		{ "bool", "toggle" }
	};
	bool ignore = false;
	vector<string> lines = split( source, '\n' );
	for( auto &it : lines ) {
		string original = it;
		string line = it;

		string ignoreStart( "/*" );
		string ignoreEnd( "*/" );
		if( !ignore && line.find( ignoreStart ) != string::npos ) {
			ignore = true;
		}

		if( ignore && line.find( ignoreEnd ) == string::npos ) {
			continue;
		}
		else {
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

		size_t foundUniform = line.find( uniform );
		if( foundUniform == string::npos ) {
			continue;
		}
		size_t foundComment = line.find( comment );
		if( foundComment == string::npos || foundUniform > foundComment ) {
			continue;
		}
		size_t foundType = string::npos;
		size_t foundUIType = string::npos;
		string type;
		string uitype;
		string key;

		bool valid = false;
		for( auto &ui : uiTypeMap ) {
			foundType = line.find( ui.first );
			//			string tempkey = comment + ui.second + colon;
			string tempkey = comment + ui.second;
			foundUIType = line.find( tempkey );
			if( foundType != string::npos && foundUIType != string::npos ) {
				valid = true;
				type = ui.first;
				uitype = ui.second;
				key = tempkey;
				break;
			}
		}

		if( !valid ) {
			continue;
		}

		string uniformName = original;
		size_t foundSemicolon = uniformName.find( semicolon );
		uniformName = uniformName.substr( 0, foundSemicolon );
		uniformName = uniformName.replace( foundType, type.length(), "" );
		uniformName = uniformName.replace( foundUniform, uniform.length(), "" );
		uniformName = trim( uniformName, " " );

		string uiParams = line.substr( foundUIType + key.length() );
		uiParams = trim( uiParams, ":" );

		vector<string> params = split( uiParams, "," );
		if( params.size() == 1 ) {
			if( params[0].length() == 0 ) {
				params.clear();
			}
		}
		int size = (int)params.size();

		vector<float> values;
		bool invalidParams = false;
		for( auto &it : params ) {
			try {
				values.emplace_back( stof( it ) );
			}
			catch( std::exception &exc ) {
				invalidParams = true;
				break;
			}
		}

		if( invalidParams ) {
			continue;
		}

		valid = false;

		if( type == "bool" ) {
			if( size == 1 ) {
				bool val = values[0] > 0.5 ? true : false;
				mBoolParams[uniformName] = val;
			}
			else {
				mBoolParams[uniformName] = true;
			}
			valid = true;
		}
		else if( type == "int" ) {
			if( size > 2 ) {
				mIntParams[uniformName] = values[2];
				mIntRanges[uniformName] = { values[0], values[1] };
			}
			else {
				mIntParams[uniformName] = 50;
				mIntRanges[uniformName] = { 0, 100 };
			}
			valid = true;
		}
		else if( type == "float" ) {
			if( size > 2 ) {
				mFloatParams[uniformName] = values[2];
				mFloatRanges[uniformName] = { values[0], values[1] };
			}
			else {
				mFloatParams[uniformName] = 0.5f;
				mFloatRanges[uniformName] = { 0.0f, 1.0f };
			}
			valid = true;
		}
		else if( type == "vec2" ) {
			if( size > 2 ) {
				if( uitype == "range" && size > 3 ) {
					mVec2Params[uniformName] = vec2( values[2], values[3] );
				}
				else {
					mVec2Params[uniformName] = vec2( values[2] );
				}
				mVec2Ranges[uniformName] = { values[0], values[1] };
			}
			else {
				mVec2Params[uniformName] = vec2( 0.25f, 0.75f );
				mVec2Ranges[uniformName] = { 0.0f, 1.0f };
			}
			valid = true;
		}
		else if( type == "vec3" ) {
			if( size > 2 ) {
				mVec3Params[uniformName] = vec3( values[2] );
				mVec3Ranges[uniformName] = { values[0], values[1] };
			}
			else {
				mVec3Params[uniformName] = vec3( 0.5f );
				mVec3Ranges[uniformName] = { 0.0f, 1.0f };
			}
			valid = true;
		}
		else if( type == "vec4" && uitype == "color" ) {
			ColorA clr;
			if( size > 3 ) {
				clr.set( ColorModel::CM_RGB, vec4( values[0], values[1], values[2], values[3] ) );
				mColorParams[uniformName] = clr;
			}
			else {
				clr.set( ColorModel::CM_RGB, vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
				mColorParams[uniformName] = clr;
			}
			valid = true;
		}
		else if( type == "vec4" ) {
			if( size > 2 ) {
				mVec4Params[uniformName] = vec4( values[2] );
				mVec4Ranges[uniformName] = { values[0], values[1] };
			}
			else {
				mVec4Params[uniformName] = vec4( 0.5f );
				mVec4Ranges[uniformName] = { 0.0f, 1.0f };
			}
			valid = true;
		}

		if( valid ) {
			mTypeMap.insert( { uniformName, { type, uitype } } );
			mParamOrder[(int)mParamOrder.size()] = uniformName;
		}
	}
}

const map<int, string> &GlslParams::getParamOrder()
{
	return mParamOrder;
}

const map<string, pair<string, string>> &GlslParams::getTypeMap()
{
	return mTypeMap;
}

map<string, bool> &GlslParams::getBoolParams()
{
	return mBoolParams;
}

map<string, int> &GlslParams::getIntParams()
{
	return mIntParams;
}
map<string, pair<int, int>> &GlslParams::getIntRanges()
{
	return mIntRanges;
}

map<string, float> &GlslParams::getFloatParams()
{
	return mFloatParams;
}
map<string, pair<float, float>> &GlslParams::getFloatRanges()
{
	return mFloatRanges;
}

map<string, vec2> &GlslParams::getVec2Params()
{
	return mVec2Params;
}
map<string, pair<float, float>> &GlslParams::getVec2Ranges()
{
	return mVec2Ranges;
}

map<string, vec3> &GlslParams::getVec3Params()
{
	return mVec3Params;
}
map<string, pair<float, float>> &GlslParams::getVec3Ranges()
{
	return mVec3Ranges;
}

map<string, vec4> &GlslParams::getVec4Params()
{
	return mVec4Params;
}
map<string, pair<float, float>> &GlslParams::getVec4Ranges()
{
	return mVec4Ranges;
}

map<string, ColorA> &GlslParams::getColorParams()
{
	return mColorParams;
}
