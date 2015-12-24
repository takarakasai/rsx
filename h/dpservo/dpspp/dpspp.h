
#ifndef DPSPP_H
#define DPSPP_H

extern "C" {
  #include "rsx.h"
  #include "ics.h"
  #include "dpservo.h"
  #include "serial/hr_serial.h"
}

#include <memory>

 errno_t deleter (dpservo_base *base) {
      printf("===================== deleter ==============>>>>\n");

      /*
      if (child_name == "rsx") {
        DPSERVO_STRUCT(rsx, kNUM_OF_JOINTS, kMAX_PKT_SIZE, RSX_DECL);
        delete (DPSERVO_STRUCT_TYPE(rsx)*)base;
      } else if (child_name == "ics") {
        DPSERVO_STRUCT(ics, kNUM_OF_JOINTS, kMAX_PKT_SIZE, ICS_DECL);
        delete (DPSERVO_STRUCT_TYPE(ics)*)base;
      } else {
        return EINVAL;
      }

      child_name.clear();
      */

      return EOK;
    }



namespace dp {

  template <size_t kNUM_OF_JOINTS, size_t kMAX_PKT_SIZE>
  class dpspp
  {
   public:
    dpspp(void) {
      //DPSERVO_INIT(servo_inst, RSX_INIT);
      //DPSERVO_INIT(servo_inst, ICS_INIT);
      //RSX_INIT(servo_inst);
      //servo = (rsx*)(&servo_inst);
    }

    ~dpspp(void) {
      //errno_t eno = dps_close(pservo);
      //if (eno != EOK) {
      //  fprintf(stderr, "rsx_close error at %s\n", __FUNCTION__);
      //}
    }

    errno_t create (const std::string name) {
      if(!child_name.empty()) {
        return -1;
      }

      if (name == "rsx") {
        DPSERVO_STRUCT(rsx, kNUM_OF_JOINTS, kMAX_PKT_SIZE, RSX_DECL) *servo;
        //servo = new DPSERVO_STRUCT_TYPE(rsx);
        //DPSERVO_PTR_INIT(servo, RSX_INIT);
        //pservo = (DPSERVO_STRUCT_TYPE(rsx) *)servo;
        //pservo = new std::unique_ptr<dpservo_base, decltype(deleter)>((DPSERVO_STRUCT_TYPE(rsx) *)servo, deleter);
        pservo = new std::unique_ptr<dpservo_base>(new DPSERVO_STRUCT_TYPE(rsx), deleter);
      } else if (name == "ics") {
        DPSERVO_STRUCT(ics, kNUM_OF_JOINTS, kMAX_PKT_SIZE, ICS_DECL) *servo;
        //servo = new DPSERVO_STRUCT_TYPE(ics);
        //DPSERVO_PTR_INIT(servo, ICS_INIT);
        //pservo = (DPSERVO_STRUCT_TYPE(ics) *)servo;
        //pservo = new std::unique_ptr<dpservo_base, decltype(deleter)>((DPSERVO_STRUCT_TYPE(ics) *)servo, deleter);
        pservo = new std::unique_ptr<dpservo_base>(new DPSERVO_STRUCT_TYPE(ics), deleter);
      } else {
        return EINVAL;
      }

      child_name = name;

      return EOK;
    }

    errno_t init (void) {
      if(child_name.empty()) {
        return -2;
      }

      if (child_name == "rsx") {
        DPSERVO_STRUCT(rsx, kNUM_OF_JOINTS, kMAX_PKT_SIZE, RSX_DECL);
        DPSERVO_PTR_INIT((DPSERVO_STRUCT_TYPE(rsx)*)pservo, RSX_INIT);
      } else if (child_name == "ics") {
        DPSERVO_STRUCT(ics, kNUM_OF_JOINTS, kMAX_PKT_SIZE, ICS_DECL);
        DPSERVO_PTR_INIT((DPSERVO_STRUCT_TYPE(ics)*)pservo, ICS_INIT);
      } else {
        return EINVAL;
      }

      return EOK;
    }

   protected: 
   private:
    std::string child_name;
    //std::unique_ptr<dpservo_base, decltype(deleter)> pservo;
    std::unique_ptr<dpservo_base> pservo;
  };
}

#endif

