#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
//#include <execinfo.h>
#include <sys/resource.h>
#endif
#include <stddef.h>
#include <vector>

#include "../src/utils/gettime.h"
#include "../src/utils/logoutput.h"
#include "../src/sliceDataStorage.h"

#include "../src/modelFile/modelFile.h"
#include "../src/settings.h"
#include "../src/optimizedModel.h"
#include "../src/multiVolumes.h"
#include "../src/polygonOptimizer.h"
#include "../src/slicer.h"
#include "../src/layerPart.h"
#include "../src/inset.h"
#include "../src/skin.h"
#include "../src/infill.h"
#include "../src/bridge.h"
#include "../src/support.h"
#include "../src/pathOrderOptimizer.h"
#include "../src/skirt.h"
#include "../src/raft.h"
#include "../src/comb.h"
#include "../src/gcodeExport.h"
#include "../src/fffProcessor.h"
#include "org_jcuraengine_JCuraEngine.h"
using namespace cura;
JNIEXPORT jlong JNICALL Java_org_jcuraengine_JCuraEngine_jniNewJCuraEngine (JNIEnv *env, jobject obj)
{
    ConfigSettings config;
    fffProcessor processor(config);
    return (jlong)&processor;
}

JNIEXPORT void JNICALL Java_org_jcuraengine_JCuraEngine_jniDispose (JNIEnv *env, jobject obj, jlong addr)
{
    fffProcessor* p = (fffProcessor*)addr;
    delete p;
}

JNIEXPORT jstring JNICALL Java_org_jcuraengine_JCuraEngine_test (JNIEnv *env, jobject obj, jstring jstr)
{
    char*   rtn   =   NULL;
        jclass   clsstring   =   env->FindClass("java/lang/String");
        jstring   strencode   =   env->NewStringUTF("UTF-8");
        jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
        jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
        jsize   alen   =   env->GetArrayLength(barr);
        jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
        if(alen   >   0)
        {
            rtn   =   (char*)malloc(alen+1);
            memcpy(rtn,ba,alen);
            rtn[alen]=0;
        }
        env->ReleaseByteArrayElements(barr,ba,0);
        std::string stemp(rtn);
        free(rtn);

        stemp += "\n JNI+++++++++++++ 哈哈哈";
        jmethodID ctorID = (env)->GetMethodID(clsstring, "<init>", "([BLjava/lang/String;)V");
        //建立byte数组
        jbyteArray bytes = (env)->NewByteArray(stemp.length());
        //将char* 转换为byte数组
        (env)->SetByteArrayRegion(bytes, 0, stemp.length(), (jbyte*)stemp.c_str());
        // 设置String, 保存语言类型,用于byte数组转换至String时的参数
        jstring encoding = (env)->NewStringUTF("UTF-8");
        //将byte数组转换为java String,并输出
        return (jstring)(env)->NewObject(clsstring, ctorID, bytes, encoding);

}
