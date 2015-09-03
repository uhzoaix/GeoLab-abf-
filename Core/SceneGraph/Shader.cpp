#include "Shader.h"
#include <GL/glew.h>
#include <QFileInfo>
#include <QDomDocument>
namespace GCL { namespace SceneGraph {

char *textFileRead(const char *fn) {


    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");

        if (fp != NULL) {

            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

Shader::Shader()
    :mIsShaderUsed(false),mIsSupported(false),mIsUpdated(true)
{
}

void Shader::init()
{
    if(!mIsShaderUsed) return;
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
        mIsShaderUsed = false;
        glGetError();
        return;
    }
    if(GLEW_ARB_vertex_program && GLEW_ARB_fragment_program)
    {
        char *fs = textFileRead(mFragFileName.toStdString().c_str());
        char *vs = textFileRead(mVertFileName.toStdString().c_str());

        if(fs == NULL || vs == NULL)
        {
            if(fs != NULL) free(fs);
            if(vs != NULL) free(vs);
            mIsSupported = false;
            return;
        }
        mIsSupported = true;
        mVertObject = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        mFragObject = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        const char * vv = vs;
        const char * ff = fs;
        glShaderSourceARB(mVertObject, 1, &vv, NULL);
        glShaderSourceARB(mFragObject, 1, &ff, NULL);

        free(fs);
        free(vs);

        glCompileShaderARB(mVertObject);
        glCompileShaderARB(mFragObject);

        GLint statusV;
        GLint statusF;

        glGetObjectParameterivARB(mVertObject, GL_OBJECT_COMPILE_STATUS_ARB, &statusV);
        glGetObjectParameterivARB(mFragObject, GL_OBJECT_COMPILE_STATUS_ARB, &statusF);

        if (statusF && statusV)  //successful compile
        {
            mShaderProg = glCreateProgramObjectARB();
            glAttachObjectARB(mShaderProg,mVertObject);
            glAttachObjectARB(mShaderProg,mFragObject);
            glLinkProgramARB(mShaderProg);

            GLint linkStatus;
            glGetObjectParameterivARB(mShaderProg, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);
            if (linkStatus) {
                for(std::vector<UniformVariable>::iterator itr= mUnifromVariableList.begin(); itr != mUnifromVariableList.end(); itr++)
                {
                    (*itr).location = glGetUniformLocationARB(mShaderProg, (*itr).name.c_str());
                }
            }
            else
            {

                char proglog[2048];
                GLsizei length;

                glGetProgramiv(mVertObject, GL_LINK_STATUS, &statusV);
                glGetProgramInfoLog(mVertObject, 2048, &length, proglog);
                ErrorLog << "VERTEX SHADER LINK INFO:" <<" "<<proglog<<std::endl;

                glGetProgramiv(mFragObject, GL_LINK_STATUS, &statusF);
                glGetProgramInfoLog(mFragObject, 2048, &length, proglog);
                ErrorLog << "FRAGMENT SHADER LINK INFO:" << proglog <<std::endl;

            }

        }
        glGetError();

    }
}
void Shader::enter()
{
    if(!mIsShaderUsed) return;
    if(!mIsUpdated)
    {
        init();
        mIsUpdated = true;
    }
    if(!mIsSupported) return;

    glUseProgramObjectARB(mShaderProg);
    // uniform variables
    for(std::vector<UniformVariable>::iterator itr= mUnifromVariableList.begin(); itr != mUnifromVariableList.end(); itr++)
    {
        switch(itr->type)
        {
        case SINGLE_INT: {
            glUniform1iARB(itr->location, itr->ival);
        } break;
        case SINGLE_FLOAT: {
            glUniform1fARB(itr->location, itr->fval[0]);
        } break;
        case ARRAY_2_FLOAT: {
            glUniform2fARB(itr->location, itr->fval[0], itr->fval[1]);
        } break;
        case ARRAY_3_FLOAT:
        {
            glUniform3fARB(itr->location, itr->fval[0], itr->fval[1], itr->fval[2]);
        } break;
        case ARRAY_4_FLOAT:
        {
            glUniform4fARB(itr->location, itr->fval[0], itr->fval[1], itr->fval[2], itr->fval[3]);
        } break;
        default: {} break;
        }
    }
    // gl status

    for(std::map<int,float>::iterator itr = mGLStatus.begin(); itr != mGLStatus.end(); itr++)
    {
        switch (itr->first) {
        case SHADE: glShadeModel((int)itr->second); break;
        case ALPHA_TEST:
        {
            if ((int)itr->second > 0.5) glEnable(GL_ALPHA_TEST);
            else glDisable(GL_ALPHA_TEST); break;
        }
        case ALPHA_FUNC:
        {
            glAlphaFunc((int)itr->second, mGLStatus[ALPHA_CLAMP]); break;
        }
            //case ALPHA_CLAMP: used in ALPHA_FUNC
        case BLENDING:
        {
            if (itr->second > 0.5)
            {
                glEnable(GL_BLEND);
            }
            else
            {
                glDisable(GL_BLEND);
            }
            break;
        }
        case BLEND_FUNC_SRC:
        {
            glBlendFunc((int)itr->second, (int)(mGLStatus[BLEND_FUNC_DST])); break;
        }
            //case BLEND_FUNC_DST: used in BLEND_FUNC_SRC
        case BLEND_EQUATION:
        {
            glBlendEquation((int)itr->second); break;
        }
        case DEPTH_TEST:
        {
            if (itr->second > 0.5)
            {
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }
            break;
        }
        case DEPTH_FUNC:
        {
            glDepthFunc((int)itr->second); break;
        }
            //case CLAMP_NEAR:
            //case CLAMP_FAR:
        case CLEAR_COLOR_R: glClearColor(itr->second,
                                         (mGLStatus[CLEAR_COLOR_G]),
                                         (mGLStatus[CLEAR_COLOR_B]),
                                         (mGLStatus[CLEAR_COLOR_A])); break;
            //case CLEAR_COLOR_G: used in CLEAR_COLOR_R
            //case CLEAR_COLOR_B: used in CLEAR_COLOR_R
            //case CLEAR_COLOR_A: used in CLEAR_COLOR_R
        }

    }
    glGetError();

}

void Shader::leave()
{
    if(!mIsShaderUsed || !mIsSupported) return;
    glUseProgramObjectARB(0);
}

void Shader::loadShaderFromGDPFile(const QString &GDPfilename)
{
    if(GDPfilename == "" || !QFileInfo::exists(GDPfilename))
    {
        this->setUseShader(false);
        return;
    }
    QFileInfo fileInfo(GDPfilename);
    QString vertFileName;
    QString fragFileName;
    QFile file(GDPfilename);
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        ErrorLog<<"Failed doc.setContent()"<<std::endl;
        this->setUseShader(false);
        return;
    }
    file.close();
    QDomElement root = doc.documentElement();
    if(root.nodeName() != "GLSLang")
    {
        ErrorLog<<"root.nodeName() != GLSLang"<<std::endl;
        this->setUseShader(false);
        return;
    }

    QDomElement elem;
    //Vertex program filename
    elem = root.firstChildElement("VPCount");
    if (!elem.isNull()) {
        //first child of VPCount is "Filenames"
        QDomNode child = elem.firstChild();
        if (!child.isNull()) {
            //first child of "Filenames" is "Filename0"
            child = child.firstChild();
            vertFileName = fileInfo.absolutePath()+"/" + (child.toElement()).attribute("VertexProgram", "");
        }
    }

    //Fragment program filename
    elem = root.firstChildElement("FPCount");
    if (!elem.isNull()) {
        //first child of FPCount is "Filenames"
        QDomNode child = elem.firstChild();
        if (!child.isNull()) {
            //first child of "Filenames" is "Filename0"
            child = child.firstChild();
            fragFileName=fileInfo.absolutePath()+"/" + ((child.toElement()).attribute("FragmentProgram", ""));
        }
    }

    if(!QFileInfo::exists(vertFileName) || !QFileInfo::exists(fragFileName))
    {
        this->setUseShader(false);
        return;
    }

    this->clear();
    this->setUseShader(true);
    this->setVertFileName(vertFileName);
    this->setFragFileName(fragFileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        ErrorLog<<"Failed file.open("<<GDPfilename.toStdString()<<std::endl;
        this->setUseShader(false);
        return;
    }

    //Uniform Variables
    elem = root.firstChildElement("UniformVariables");
    if (!elem.isNull()) {

        QDomNode unif = elem.firstChild();
        while( !unif.isNull() ) {

            UniformVariable uv;

            QDomElement unifElem = unif.toElement();
            QString unifVarName = unifElem.attribute("Name", "");

            uv.type = (unifElem.attribute("Type", "")).toInt();
            uv.widget = (unifElem.attribute("Widget", "")).toInt();
            uv.min = (unifElem.attribute("Min", "")).toFloat();
            uv.max = (unifElem.attribute("Max", "")).toFloat();
            uv.step = (unifElem.attribute("Step", "")).toFloat();

            QDomNode unifElemValue = unifElem.firstChild();

            if (!unifElemValue.isNull()) {

                switch (uv.type)
                {
                case SINGLE_INT:
                {
                    uv.ival = unifElemValue.toElement().attribute("Value0", 0).toInt();
                } break;
                case SINGLE_FLOAT:
                {
                    uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();
                } break;
                case ARRAY_2_FLOAT:
                {
                    uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();
                    uv.fval[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();
                } break;
                case ARRAY_3_FLOAT:
                {
                    uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();
                    uv.fval[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();
                    uv.fval[2] = unifElemValue.toElement().attribute("Value2", 0).toFloat();
                } break;
                case ARRAY_4_FLOAT:
                {
                    uv.fval[0] = unifElemValue.toElement().attribute("Value0", 0).toFloat();
                    uv.fval[1] = unifElemValue.toElement().attribute("Value1", 0).toFloat();
                    uv.fval[2] = unifElemValue.toElement().attribute("Value2", 0).toFloat();
                    uv.fval[3] = unifElemValue.toElement().attribute("Value3", 0).toFloat();
                } break;
                default:
                {

                } break;
                }

                uv.name = unifVarName.toStdString();
                this->appendUniformVariable(uv);

            }

            unif = unif.nextSibling();
        }
    }


    //OpenGL Status
    elem = root.firstChildElement("FragmentProcessor");
    if (!elem.isNull()) {
        if (elem.hasAttribute("Shade"))					this->glStatus()[SHADE] = elem.attribute("Shade", "0").toFloat();
        if (elem.hasAttribute("AlphaTest"))			this->glStatus()[ALPHA_TEST] = (elem.attribute("AlphaTest", "False")=="False")?0.f:1.f;
        if (elem.hasAttribute("AlphaFunc"))			this->glStatus()[ALPHA_FUNC] = elem.attribute("AlphaFunc", "0").toFloat();
        if (elem.hasAttribute("AlphaClamp"))		this->glStatus()[ALPHA_CLAMP] = elem.attribute("AlphaClamp", "0").toFloat();
        if (elem.hasAttribute("Blending"))			this->glStatus()[BLENDING] = (elem.attribute("Blending", "False")=="False")?0.f:1.f;
        if (elem.hasAttribute("BlendFuncSRC"))	this->glStatus()[BLEND_FUNC_SRC] = elem.attribute("BlendFuncSRC", "0").toFloat();
        if (elem.hasAttribute("BlendFuncDST"))	this->glStatus()[BLEND_FUNC_DST] = elem.attribute("BlendFuncDST", "0").toFloat();
        if (elem.hasAttribute("BlendEquation")) this->glStatus()[BLEND_EQUATION] = elem.attribute("BlendEquation", "0").toFloat();
        if (elem.hasAttribute("DepthTest"))			this->glStatus()[DEPTH_TEST] = (elem.attribute("DepthTest", "False")=="False")?0.f:1.f;
        if (elem.hasAttribute("DepthFunc"))			this->glStatus()[DEPTH_FUNC] = elem.attribute("DepthFunc", "0").toFloat();
        if (elem.hasAttribute("ClampNear"))			this->glStatus()[CLAMP_NEAR] = elem.attribute("ClampNear", "0").toFloat();
        if (elem.hasAttribute("ClampFar"))			this->glStatus()[CLAMP_FAR] = elem.attribute("ClampFar", "0").toFloat();
        if (elem.hasAttribute("ClearColorR"))		this->glStatus()[CLEAR_COLOR_R] = elem.attribute("ClearColorR", "0").toFloat();
        if (elem.hasAttribute("ClearColorG"))		this->glStatus()[CLEAR_COLOR_G] = elem.attribute("ClearColorG", "0").toFloat();
        if (elem.hasAttribute("ClearColorB"))		this->glStatus()[CLEAR_COLOR_B] = elem.attribute("ClearColorB", "0").toFloat();
        if (elem.hasAttribute("ClearColorA"))		this->glStatus()[CLEAR_COLOR_A] = elem.attribute("ClearColorA", "0").toFloat();
    }
    this->update();
}

}}
