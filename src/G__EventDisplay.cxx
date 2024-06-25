// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIUsersdIsiyuandIPhysicsdIcometdIcrtdItrackingdIsrcdIG__EventDisplay
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
#include "/Users/siyuan/Physics/comet/crt/tracking/src/EventDisplay.hxx"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *EventDisplay_Dictionary();
   static void EventDisplay_TClassManip(TClass*);
   static void delete_EventDisplay(void *p);
   static void deleteArray_EventDisplay(void *p);
   static void destruct_EventDisplay(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::EventDisplay*)
   {
      ::EventDisplay *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::EventDisplay));
      static ::ROOT::TGenericClassInfo 
         instance("EventDisplay", "src/EventDisplay.hxx", 21,
                  typeid(::EventDisplay), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &EventDisplay_Dictionary, isa_proxy, 0,
                  sizeof(::EventDisplay) );
      instance.SetDelete(&delete_EventDisplay);
      instance.SetDeleteArray(&deleteArray_EventDisplay);
      instance.SetDestructor(&destruct_EventDisplay);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::EventDisplay*)
   {
      return GenerateInitInstanceLocal(static_cast<::EventDisplay*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::EventDisplay*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *EventDisplay_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::EventDisplay*>(nullptr))->GetClass();
      EventDisplay_TClassManip(theClass);
   return theClass;
   }

   static void EventDisplay_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_EventDisplay(void *p) {
      delete (static_cast<::EventDisplay*>(p));
   }
   static void deleteArray_EventDisplay(void *p) {
      delete [] (static_cast<::EventDisplay*>(p));
   }
   static void destruct_EventDisplay(void *p) {
      typedef ::EventDisplay current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::EventDisplay

namespace {
  void TriggerDictionaryInitialization_libG__EventDisplay_Impl() {
    static const char* headers[] = {
"/Users/siyuan/Physics/comet/crt/tracking/src/EventDisplay.hxx",
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
#line 1 "libG__EventDisplay dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$/Users/siyuan/Physics/comet/crt/tracking/src/EventDisplay.hxx")))  EventDisplay;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libG__EventDisplay dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "/Users/siyuan/Physics/comet/crt/tracking/src/EventDisplay.hxx"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"EventDisplay", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libG__EventDisplay",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libG__EventDisplay_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libG__EventDisplay_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libG__EventDisplay() {
  TriggerDictionaryInitialization_libG__EventDisplay_Impl();
}
