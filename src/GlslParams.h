/*
Copyright(c) 2017 Reza Ali syed.reza.ali@gmail.com www.syedrezaali.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <map>
#include <string>
#include <vector>

#include "cinder/gl/GlslProg.h"

namespace reza {
namespace glsl {

typedef std::shared_ptr<class GlslParams> GlslParamsRef;
class GlslParams {
  public:
	static GlslParamsRef create( const std::string &source )
	{
		return GlslParamsRef( new GlslParams( source ) );
	}

	static GlslParamsRef create()
	{
		return GlslParamsRef(new GlslParams());
	}

	GlslParams();
	GlslParams( const std::string &source );
	GlslParams( const GlslParams &copy );
	~GlslParams();

	void clearUniforms();

	void applyUniforms( const ci::gl::GlslProgRef &glslRef );
	void parseUniforms( const std::vector<std::string> &sources );
	void parseUniforms( const std::string &source );
	void parse( const std::string &source );

	const std::map<int, std::string> &getParamOrder();
	const std::map<std::string, std::pair<std::string, std::string>> &getTypeMap();

	std::map<std::string, bool> &getBoolParams();

	std::map<std::string, int> &getIntParams();
	std::map<std::string, std::pair<int, int>> &getIntRanges();

	std::map<std::string, float> &getFloatParams();
	std::map<std::string, std::pair<float, float>> &getFloatRanges();

	std::map<std::string, glm::vec2> &getVec2Params();
	std::map<std::string, std::pair<float, float>> &getVec2Ranges();

	std::map<std::string, glm::vec3> &getVec3Params();
	std::map<std::string, std::pair<float, float>> &getVec3Ranges();

	std::map<std::string, glm::vec4> &getVec4Params();
	std::map<std::string, std::pair<float, float>> &getVec4Ranges();

	std::map<std::string, ci::ColorA> &getColorParams();

  protected:
	std::map<int, std::string> mParamOrder;
	std::map<std::string, std::pair<std::string, std::string>> mTypeMap;

	std::map<std::string, bool> mBoolParams;

	std::map<std::string, float> mFloatParams;
	std::map<std::string, std::pair<float, float>> mFloatRanges;

	std::map<std::string, int> mIntParams;
	std::map<std::string, std::pair<int, int>> mIntRanges;

	std::map<std::string, glm::vec2> mVec2Params;
	std::map<std::string, std::pair<float, float>> mVec2Ranges;

	std::map<std::string, glm::vec3> mVec3Params;
	std::map<std::string, std::pair<float, float>> mVec3Ranges;

	std::map<std::string, glm::vec4> mVec4Params;
	std::map<std::string, std::pair<float, float>> mVec4Ranges;

	std::map<std::string, ci::ColorA> mColorParams;
};
}
} // namespace reza::glsl