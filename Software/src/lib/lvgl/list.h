#ifndef LASERSCANNINGPROJECT_LIST_H
#define LASERSCANNINGPROJECT_LIST_H

#include <lvgl.h>
#include <string_view>
#include "object.h"
#include "button.h"

namespace lvgl {

/**
 * \brief Abstraction for LVGL list.
 */
class list : public object {
 public:
  explicit list(lv_obj_t* parent = lv_scr_act(), const list* copy = nullptr);
  explicit list(lv_obj_t* handle);
  button add_btn(void* img, std::string_view txt);
  button add_btn(std::string_view txt) { return add_btn(nullptr, txt); }
};

}
#endif //LASERSCANNINGPROJECT_LIST_H
