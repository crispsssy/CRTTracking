// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIUsersdIsiyuandIPhysicsdIcometdIcrtdItrackingdIsrcdIG__STL_Dictionary
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "/Users/siyuan/Physics/comet/crt/tracking/src/stl_dictionary.hxx"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *vectorlEvectorlEintgRsPgR_Dictionary();
   static void vectorlEvectorlEintgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlEintgRsPgR(void *p = nullptr);
   static void *newArray_vectorlEvectorlEintgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlEintgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlEintgRsPgR(void *p);
   static void destruct_vectorlEvectorlEintgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<int> >*)
   {
      vector<vector<int> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<int> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<int> >", -2, "vector", 493,
                  typeid(vector<vector<int> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEvectorlEintgRsPgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<vector<int> >) );
      instance.SetNew(&new_vectorlEvectorlEintgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlEintgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlEintgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlEintgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlEintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<int> > >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<vector<int> >","std::__1::vector<std::__1::vector<int, std::__1::allocator<int> >, std::__1::allocator<std::__1::vector<int, std::__1::allocator<int> > > >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<vector<int> >*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlEintgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<vector<int> >*>(nullptr))->GetClass();
      vectorlEvectorlEintgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlEintgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlEintgRsPgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<vector<int> > : new vector<vector<int> >;
   }
   static void *newArray_vectorlEvectorlEintgRsPgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<vector<int> >[nElements] : new vector<vector<int> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlEintgRsPgR(void *p) {
      delete (static_cast<vector<vector<int> >*>(p));
   }
   static void deleteArray_vectorlEvectorlEintgRsPgR(void *p) {
      delete [] (static_cast<vector<vector<int> >*>(p));
   }
   static void destruct_vectorlEvectorlEintgRsPgR(void *p) {
      typedef vector<vector<int> > current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<vector<int> >

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = nullptr);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 493,
                  typeid(vector<int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int>","std::__1::vector<int, std::__1::allocator<int> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr))->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete (static_cast<vector<int>*>(p));
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] (static_cast<vector<int>*>(p));
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<int>

namespace ROOT {
   static TClass *vectorlETVector3gR_Dictionary();
   static void vectorlETVector3gR_TClassManip(TClass*);
   static void *new_vectorlETVector3gR(void *p = nullptr);
   static void *newArray_vectorlETVector3gR(Long_t size, void *p);
   static void delete_vectorlETVector3gR(void *p);
   static void deleteArray_vectorlETVector3gR(void *p);
   static void destruct_vectorlETVector3gR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TVector3>*)
   {
      vector<TVector3> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TVector3>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TVector3>", -2, "vector", 493,
                  typeid(vector<TVector3>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETVector3gR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TVector3>) );
      instance.SetNew(&new_vectorlETVector3gR);
      instance.SetNewArray(&newArray_vectorlETVector3gR);
      instance.SetDelete(&delete_vectorlETVector3gR);
      instance.SetDeleteArray(&deleteArray_vectorlETVector3gR);
      instance.SetDestructor(&destruct_vectorlETVector3gR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TVector3> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<TVector3>","std::__1::vector<TVector3, std::__1::allocator<TVector3> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<TVector3>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETVector3gR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<TVector3>*>(nullptr))->GetClass();
      vectorlETVector3gR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETVector3gR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETVector3gR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<TVector3> : new vector<TVector3>;
   }
   static void *newArray_vectorlETVector3gR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<TVector3>[nElements] : new vector<TVector3>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETVector3gR(void *p) {
      delete (static_cast<vector<TVector3>*>(p));
   }
   static void deleteArray_vectorlETVector3gR(void *p) {
      delete [] (static_cast<vector<TVector3>*>(p));
   }
   static void destruct_vectorlETVector3gR(void *p) {
      typedef vector<TVector3> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<TVector3>

namespace {
  void TriggerDictionaryInitialization_libG__STL_Dictionary_Impl() {
    static const char* headers[] = {
"/Users/siyuan/Physics/comet/crt/tracking/src/stl_dictionary.hxx",
nullptr
    };
    static const char* includePaths[] = {
"/Users/siyuan/Physics/root/install/include",
"/Users/siyuan/Physics/comet/crt/tracking",
"/Users/siyuan/Physics/root/install/include/",
"/Users/siyuan/Physics/comet/crt/tracking/build/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libG__STL_Dictionary dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
namespace std{inline namespace __1{template <class _Tp> class __attribute__((annotate("$clingAutoload$iosfwd")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}}
class __attribute__((annotate("$clingAutoload$TVector3.h")))  __attribute__((annotate("$clingAutoload$/Users/siyuan/Physics/comet/crt/tracking/src/stl_dictionary.hxx")))  TVector3;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libG__STL_Dictionary dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "/Users/siyuan/Physics/comet/crt/tracking/src/stl_dictionary.hxx"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libG__STL_Dictionary",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libG__STL_Dictionary_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libG__STL_Dictionary_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libG__STL_Dictionary() {
  TriggerDictionaryInitialization_libG__STL_Dictionary_Impl();
}
