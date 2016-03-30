APP_ABI := all
#C++ 运行库的名字 :	 \
system:默认的微型系统c++运行库。若APP_STL未设置，系统会作为默认值使用 \
gabi++_static: 作为静态库的GAbi++ 运行库 \
gabi++_shared: 作为共享库的GAbi++运行库\
stlport_static: 作为静态库的STLport运行库\
stlport_shared:	作为共享库的STLport运行库\
gnustl_static:作为静态库的GUN STL运行库\
gnustl_shared:作为共享库的GUN STL运行库\
当C++运行库以共享库的形式使用时:应用程序需要预先加载所需要的共享库，然后在加载依赖于共享库的其他原生模块\
例如: \
static{ \
 System.loadLibrary("stlport_shared"); \
 System.loadLibrary("module1"); \
 System.loadLibrary("module2");\
 }
APP_STL :=gnustl_shared
#使用  -frtt 添加 rtti支持   -fexceptions 添加exceptions支持
LOCAL_CPP_FEATURES += -frtti