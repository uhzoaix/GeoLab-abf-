#ifndef GCL_SHADER_H
#define GCL_SHADER_H
#include <vector>
#include <QString>
#include <map>
#include "Core/Utilities/LogSystem.h"
#include "Core/gcl_global.h"
namespace GCL { namespace SceneGraph {

enum {
  SINGLE_INT = 1,
  SINGLE_FLOAT = 5,
  ARRAY_2_FLOAT = 6,
  ARRAY_3_FLOAT = 7,
  ARRAY_4_FLOAT = 8
};

enum {
    WIDGET_NONE = 0,
    WIDGET_COLOR = 1,
    WIDGET_SLIDER = 2
};

enum {
    SHADE = 0,
    ALPHA_TEST,
    ALPHA_FUNC,
    ALPHA_CLAMP,
    BLENDING,
    BLEND_FUNC_SRC,
    BLEND_FUNC_DST,
    BLEND_EQUATION,
    DEPTH_TEST,
    DEPTH_FUNC,
    CLAMP_NEAR,
    CLAMP_FAR,
    CLEAR_COLOR_R,
    CLEAR_COLOR_G,
    CLEAR_COLOR_B,
    CLEAR_COLOR_A
};

struct UniformVariable
{
    short type;
    short widget;
    float min;
    float max;
    float step;
    int location;
    int ival;
    float fval[4];
    std::string name;
    int getVarNumber() const
    {
        switch(type)
        {
        case SINGLE_INT:case SINGLE_FLOAT: return 1;break;
        case ARRAY_2_FLOAT:return 2;break;
        case ARRAY_3_FLOAT:return 3;break;
        case ARRAY_4_FLOAT:return 4;break;
        default:return 0; break;
        }
        return 0;
    }
};
/**
 * @brief The Shader class
 *      Set shader for scenegraph;
 * @author Shiwei Wang
 * @date 2014.8.9
 */

class GCL_CORE_SHARED_EXPORT Shader
{
public:
    Shader();
    ~Shader() {}
    void clear() {mUnifromVariableList.clear();mGLStatus.clear();}
    std::map< int, float>& glStatus() {return mGLStatus;}

    int getUniformVariablesNumber() const { return mUnifromVariableList.size();}
    void appendUniformVariable(const UniformVariable& uv) {mUnifromVariableList.push_back(uv);}
    UniformVariable getUniformVariable(int index) const {return mUnifromVariableList[index];}
    UniformVariable& getUniformVariable(int index) {return mUnifromVariableList[index];}
    void setVertFileName(const QString& name) {mVertFileName = name;}
    void setFragFileName(const QString& name) {mFragFileName = name;}
    void setUseShader(bool t) {mIsShaderUsed = t;}
    void init();
    void enter();
    void leave();

    void update() {mIsUpdated = false;}

    void loadShaderFromGDPFile(const QString &GDPfilename);
private:
    std::vector< UniformVariable > mUnifromVariableList;
    std::map< int, float> mGLStatus;
    QString mVertFileName;
    QString mFragFileName;
    QString mGDPFileName;
    bool mIsShaderUsed;
    bool mIsSupported;
    bool mIsUpdated;


    unsigned int mVertObject;
    unsigned int mFragObject;
    unsigned int mShaderProg;




};
}}
#endif // SHADER_H
