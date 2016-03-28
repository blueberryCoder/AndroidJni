/**模块名是Unix. */
%module Unix

%{
	/**包含POSIX操作系统API.*/
	#include <unistd.h>
%}

/**告诉SWIG uid_t.*/
//typedef unsigned int uid_t;

/* 让SIWG包装getuid函数.*/
//extern uid_t getuid(void);

/**全局变量*/
extern int counter;
/**用定义指令定义常量*/
%javaconst(1);
/**用define指令定义的常量*/
#define MAX_WIDTH 640
 	
/**用定义指令定义常量*/
%javaconst(0);
/**用%constant 指令定义的常量*/
%constant int MAX_HEIGHT = 320;

/**启用只读模式*/
%immutable;
/**只读变量*/
extern int readOnly;
/**禁用只读模式*/
%mutable;
/* 读-写变量*/
extern int readWrite;

/**匿名枚举*/
//enum {ONE=1,TWO=2,THREE,FOUR};

/**命名枚举*/
//enum Numbers {ONE=1,TWO=2,THREE,FOUR};

/**类型不安全枚举*/
//%include "enumtypeunsafe.swg"

/**命名枚举. */
//enum Numbers {ONE=1,TWO=2,THREE,FOUR};

/**生成java枚举类*/
//%include "enums.swg"
/**命名枚举*/
//enum Numbers {ONE=1,TWO=2,THREE,FOUR};
/**结构体*/

struct Point {
	int x;
	int y;
};
//c++
/**通过指针*/
void drawByPointer(struct Point* p);

/**通过引用*/
void drawByReference(struct Point& p);

/**通过值*/
void drawByValue(struct Point p);

/**默认参数*/
void func(int a=1,int b=2,int c=3);

/**重载函数*/
void func(double d);

void func(int i );

/**类*/
class A {
	public:
		A();
		A(int value);
		~A();
		void print();
		int value ;
	private:
		void reset();
};
/*

%exception getuid {
	$action
		if(!result){
				jclass clazz = jenv->FindClass("java/lang/OutOfMemoryEror");
				jenv-ThrowNew(clazz,"Out of Memory");
				return $null;
			}
}
*/
%javaexception("java.lang.IllegalAccessException") getuid{
	$action
	if(!result){
		jclass clazz = jenv->FindClass("java/lang/IllegalAccessException");
		jenv->ThrowNew(clazz,"Illegal Access");
		return $null;
	}
}
/**请求SIWG封装的getuid函数**/
extern uid_t getuid(void);

/**
从原声代码中调用java
*/

/**启用对交叉语言多态性的支持*/
%module(directors=1) Unix2

/*启用AsyncUidProvider的directors*/
%feature("director") AsyncUidProvider
%{
/**Asynchornous user ID 提供着*/
class AsyncUidProvider{
	public:
		AsyncUidProvider(){
			
		}
		virtual ~AsyncUidProvider(){
			
		}
		void get(){
			onUid(getuid());
		}
		virtual void onUid(uid_t uid){
		}
};
%}

/**Asynchorous user ID provider.*/
class AsyncUidProvider{
	public:
		AsyncUidProvider();
		virtual ~AsyncUidProvider();
		
		void get();
		virtual void onUid(uid_t uid);
};


