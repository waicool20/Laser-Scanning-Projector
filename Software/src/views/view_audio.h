#ifndef LASERSCANNINGPROJECT_VIEW_AUDIO_H
#define LASERSCANNINGPROJECT_VIEW_AUDIO_H

#include "lib/lvgl/list.h"
#include "lib/lvgl/group.h"

class view_audio {
  public:
    static void show(bool show);
  private:
    static void init();
    static bool created;
    static lvgl::list& list();
    static lvgl::group& input_group();
    static void mic_btn_cb(lv_obj_t *obj, lv_event_t event);
    static void back_btn_cb(lv_obj_t *obj, lv_event_t event);
};


#endif //LASERSCANNINGPROJECT_VIEW_AUDIO_H
